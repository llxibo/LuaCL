#ifndef __LUACL_BUFFER_HPP
#define __LUACL_BUFFER_HPP

#include "LuaCL.h"
#include "luacl_object.hpp"
#include "luacl_endianness.hpp"

static const char LUACL_MEM_REGISTRY[] = "LuaCL_Buffer_Registry";
static const char LUACL_MEM_METATABLE[] = "LuaCL_Buffer";
static const char LUACL_MEM_TOSTRING[] = "LuaCL_Buffer";
static const size_t LUACL_BUFFER_MIN_SIZE = 2;

/* Buffer object, keeping cl_mem object and allocated memory. */
struct luacl_buffer_object {
    cl_mem mem = NULL;
    void * data = NULL;
    size_t size = 0;

    cl_int ReleaseMem() {
        if (mem == NULL) {
            return CL_SUCCESS;
        }
        cl_int err = clReleaseMemObject(mem);
        mem = NULL;
        return err;
    }

    ~luacl_buffer_object() {
        ReleaseMem();
        free(data);             /* According to C99, free(NULL) should always be safe. */
        data = NULL;
    }
};

typedef luacl_buffer_object* luacl_buffer_info;

template <>
struct luacl_object_constants<luacl_buffer_info> {
    static const char * REGISTRY() {
        return LUACL_MEM_REGISTRY;
    }
    static const char * METATABLE() {
        return LUACL_MEM_METATABLE;
    }
    static const char * TOSTRING() {
        return LUACL_MEM_TOSTRING;
    }
    static cl_int Release(luacl_buffer_info mem) {
        cl_int err = mem->ReleaseMem(); /* The mem destructor will also release mem. In order to get err code, release it prior to destruction. */
        delete mem;
        return err;
    }
};

struct luacl_buffer {
    typedef luacl_object<luacl_buffer_info> traits;

    static void Init(lua_State *L) {
        traits::CreateMetatable(L);
        traits::RegisterFunction(L, GetBufferSize, "GetBufferSize");
        traits::RegisterFunction(L, Clear, "Clear");
        RegisterType<int>(L, "Int");
        RegisterType<float>(L, "Float");
        RegisterType<double>(L, "Double");
        RegisterType<short>(L, "Short");
        RegisterType<char>(L, "Char");
        lua_setfield(L, -2, "__index");
        traits::RegisterRelease(L);
        traits::CreateRegistry(L);
    }

    static int Create(lua_State *L) {
        cl_context context = luacl_object<cl_context>::CheckObject(L, 1);
        size_t size = static_cast<size_t>(luaL_checknumber(L, 2));
        cl_mem_flags flags = static_cast<cl_mem_flags>(lua_tonumber(L, 3));
        flags = (flags == 0) ? (CL_MEM_READ_WRITE) : flags;

        if (LUACL_UNLIKELY(size == 0)) {
            return luaL_error(L, "Bad argument #2: size of buffer must be greater than %d.", LUACL_BUFFER_MIN_SIZE);
        }
        void * data = malloc(size);
        CheckAllocError(L, data);
        memset(data, 0, size);
        luacl_buffer_info bufferObject = new luacl_buffer_object;
        bufferObject->data = data;      /* The allocated memory is now guarded by bufferObject */
        bufferObject->size = size;
        traits::Wrap(L, bufferObject);  /* Push the pointer of uncompleted buffer object to lua stack */

        cl_int err = 0;
        cl_mem mem = clCreateBuffer(context, flags, size, NULL, &err);
        CheckCLError(L, err, "Failed creating buffer: %s.");    /* Potential function exit, *data will be released by bufferObject destructor */
        bufferObject->mem = mem;
        return 1;
    }

    static int GetBufferSize(lua_State *L) {
        luacl_buffer_info buffer = traits::CheckObject(L);
        lua_pushnumber(L, static_cast<lua_Number>(buffer->size));
        return 1;
    }

    template <typename T>
    static void RegisterType(lua_State *L, const char * name) {
        traits::RegisterFunction(L, Get<T>, std::string("Get").append(name).c_str());
        traits::RegisterFunction(L, Set<T>, std::string("Set").append(name).c_str());
        traits::RegisterFunction(L, GetSize<T>, std::string("GetSize").append(name).c_str());
        traits::RegisterFunction(L, ReverseEndian<T>, std::string("ReverseEndian").append(name).c_str());
    }

    template <typename T>
    static int Get(lua_State *L) {
        luacl_buffer_info buffer = traits::CheckObject(L, 1);
        ptrdiff_t index = static_cast<ptrdiff_t>(lua_tonumber(L, 2));

        if (index < 0 || (index + 1) * sizeof(T) > buffer->size) {
            luaL_error(L, "Buffer access out of bound.");
        }
        T * data = reinterpret_cast<T *>(buffer->data);
        lua_pushnumber(L, static_cast<lua_Number>(data[index]));
        return 1;
    }

    template <typename T>
    static int Set(lua_State *L) {
        luacl_buffer_info buffer = traits::CheckObject(L, 1);
        ptrdiff_t index = static_cast<ptrdiff_t>(lua_tonumber(L, 2));
        T value = static_cast<T>(lua_tonumber(L, 3));

        if (index < 0 || (index + 1) * sizeof(T) > buffer->size) {
            luaL_error(L, "Buffer access out of bound.");
        }
        T * data = reinterpret_cast<T *>(buffer->data);
        data[index] = value;
        //* (reinterpret_cast<T *>(buffer->data) + addr) = value; /* Compare de-asm for performance? */
        return 0;
    }

    template <typename T>
    static int GetSize(lua_State *L) {
        lua_pushnumber(L, static_cast<lua_Number>(sizeof(T)));
        return 1;
    }

    template <typename T>
    static int ReverseEndian(lua_State *L) {
        luacl_buffer_info buffer = traits::CheckObject(L, 1);
        ptrdiff_t index = static_cast<ptrdiff_t>(lua_tonumber(L, 2));
        ptrdiff_t range = static_cast<ptrdiff_t>(lua_tonumber(L, 3));
        range = (range == 0) ? buffer->size / sizeof(T) - index : range;

        if (index < 0 || range < 0 || (index + 1) * sizeof(T) > buffer->size) {
            luaL_error(L, "Buffer access out of bound.");
        }
        T * data = reinterpret_cast<T *>(buffer->data);
        LUACL_TRYCALL(
            util::luacl_byte_order_reverse<T>(data + index, range);
        );
        return 0;
    }

    static int Clear(lua_State *L) {
        luacl_buffer_info buffer = traits::CheckObject(L);
        size_t offset = static_cast<size_t>(lua_tonumber(L, 2));
        size_t bytes = static_cast<size_t>(lua_tonumber(L, 3));
        bytes = (bytes == 0) ? (buffer->size - offset) : bytes;
        if (bytes + offset > buffer->size) {
            luaL_error(L, "Bad argument: size and offset range out of bound");
        }
        memset(reinterpret_cast<char *>(buffer->data) + offset, 0, bytes);
        return 0;
    }
};

#endif /* __LUACL_BUFFER_HPP */
