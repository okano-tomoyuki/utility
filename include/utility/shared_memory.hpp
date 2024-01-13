#ifndef _UTILITY_SHARED_MEMORY_HPP_
#define _UTILITY_SHARED_MEMORY_HPP_

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <chrono>
#include <sstream>

#ifdef __unix__
#include <array>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/sem.h>
#else
#include <stdio.h>
#include <windows.h>
#endif

#ifdef GLOBAL_USE_BUILD_LIBLARY

namespace Utility
{

class SharedMemory final
{

#ifdef __unix__
using Handle = int;
#else
using Handle = HANDLE;
#endif

private:
    Handle shmem_handle_;
    Handle mutex_handle_;
    char* data_;
    int buffer_size_;
    bool is_persistence_;

#ifdef __unix__
    /**! 
     * ftokの代替として文字列から4byteハッシュ値を生成するために使用。
     * https://norizn.hatenablog.com/entry/2020/10/18/145628
     */
    static uint32_t make_hash(const char* str, const size_t& size);
#endif
    static Handle create_mutex(const char* mutex_name);

public:

    explicit SharedMemory(const char* shmem_name, const size_t& size, const char* mutex_name = "");
    ~SharedMemory();
    Handle mutex() const { return mutex_handle_; }
    bool wait_for_single_object(const Handle& mutex_handle, const int& timeout_msec = 0) const;
    void release_mutex(const Handle& mutex_handle) const;

    template<typename T> 
    T* get() { return (T*)data_; };

    template<typename T>
    bool try_write(const T* data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;
        std::memcpy(data_, data, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }

    template<typename T>
    bool try_read(T* data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;
        std::memcpy(data, data_, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }

    template<typename T>
    bool try_write(const T& data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;
        std::memcpy(data_, &data, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }

    template<typename T>
    bool try_read(T& data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;
        std::memcpy(&data, data_, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }
};

}

#else

namespace Utility
{

class SharedMemory final
{

#ifdef __unix__
using Handle = int;
#else
using Handle = HANDLE;
#endif

private:
    Handle shmem_handle_;
    Handle mutex_handle_;
    char* data_;
    int buffer_size_;

#ifdef __unix__

    static uint32_t make_hash(const char* str, const size_t& size)
    {
        const uint32_t CRC32_TABLE[256] = {
            0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9,
            0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
            0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61,
            0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
            0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9,
            0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
            0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011,
            0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
            0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
            0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
            0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81,
            0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
            0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49,
            0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
            0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1,
            0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
            
            0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE,
            0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
            0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
            0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
            0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE,
            0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
            0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066,
            0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
            0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E,
            0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
            0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6,
            0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
            0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
            0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
            0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686,
            0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
            
            0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637,
            0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
            0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F,
            0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
            0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47,
            0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
            0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
            0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
            0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7,
            0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
            0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F,
            0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
            0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7,
            0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
            0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F,
            0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,

            0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
            0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
            0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8,
            0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
            0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30,
            0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
            0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088,
            0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
            0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0,
            0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
            0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
            0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
            0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0,
            0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
            0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668,
            0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
        };
        uint32_t digest = 0xffffffff;
        for (size_t i = 0; i < size; i++)
            digest = (digest << 8) ^ CRC32_TABLE[((digest >> 24) ^ str[i]) & 0xff];
        return digest;        
    }

#endif

    static Handle create_mutex(const char* mutex_name) 
    {
#ifdef __unix__
        bool is_first = false;
        key_t key = make_hash(mutex_name, std::string(mutex_name).size());
        Handle mutex_handle = semget(key, 1, 0660); 
        if(mutex_handle == -1)
        {
            is_first = true;
            mutex_handle = semget(key, 1, IPC_CREAT | IPC_EXCL | 0660);
            if(mutex_handle == -1)
            {
                std::stringstream ss;
                ss << "semget failed. error code : " << errno     << std::endl;
                ss << "semaphore name : " << mutex_name << std::endl;
                throw std::runtime_error(ss.str());
            }
        }
        if(is_first)
        {
            union Semun 
            {
                int val;
                struct semid_ds* buf;
                unsigned short* array;
            };
            union Semun arg;
            arg.val = 1;
            semctl(mutex_handle, 0, SETVAL, arg);
        }
#else
        std::string temp(mutex_name);
        int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
        std::wstring fname(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &fname[0], size);
        Handle mutex_handle = CreateMutexW(NULL, FALSE, fname.c_str());
        if(mutex_handle==NULL)
        {
            std::stringstream ss;
            ss << "CreateMutex failed. error code : " << GetLastError()  << std::endl;
            ss << "mutex name : " << mutex_name << std::endl;
            throw std::runtime_error(ss.str());                
        }
#endif
        return mutex_handle;
    }

public:

    explicit SharedMemory(const char* shmem_name, const size_t& size, const char* mutex_name = "")
     :  buffer_size_(size)
    {
        bool is_first = false;
#ifdef __unix__
        key_t key = make_hash(shmem_name, std::string(shmem_name).length());
        shmem_handle_ = shmget(key, 0, 0); 
        if(shmem_handle_ == -1)
        {
            is_first = true;
            shmem_handle_ = shmget(key, buffer_size_, IPC_CREAT | IPC_EXCL | 0660);
            if(shmem_handle_ == -1)
            {
                std::stringstream ss;
                ss << "shmget failed. error code : " << errno << std::endl;
                throw std::runtime_error(ss.str());
            }
        }  
        data_ = (char*)shmat(shmem_handle_, 0, 0);
        if(data_ == (void*)-1)
        {
            std::stringstream ss;
            ss << "shmat failed. error code : " << errno << std::endl;
            ss << "shared memory name       : " << shmem_name << std::endl;
            throw std::runtime_error(ss.str());            
        }
#else
        std::string temp(shmem_name);
        int str_size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
        std::wstring fname(str_size, 0);
        MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &fname[0], size);
        shmem_handle_ = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, buffer_size_, fname.c_str());
        if (shmem_handle_ == 0)
        {
            std::stringstream ss;
            ss << "CreateFileMappingW failed. error code : " << GetLastError()  << std::endl;
            ss << "shmem name : " << shmem_name << std::endl;
            throw std::runtime_error(ss.str());                
        }
        data_ = (char*)MapViewOfFile(shmem_handle_, FILE_MAP_ALL_ACCESS, 0, 0, buffer_size_);
        if (data_ == NULL)
        {
            std::stringstream ss;
            ss << "MapViewOfFile failed. error code : " << GetLastError()  << std::endl;
            ss << "shmem name : " << shmem_name << std::endl;
            throw std::runtime_error(ss.str());                
        }
#endif
        if(is_first)
            std::memset(data_, 0, buffer_size_);
        std::string mutex_name_str = (mutex_name!=nullptr) ? mutex_name: std::string(shmem_name) + "_MTX";
        mutex_handle_ = create_mutex(mutex_name_str.c_str());        
    }

    ~SharedMemory()
    {
#ifdef __unix__
        shmdt(data_);        
        struct shmid_ds shm_ds;
        shmctl(shmem_handle_, IPC_STAT, &shm_ds);
        if(shm_ds.shm_nattch == 0)
        {
            shmctl(shmem_handle_, IPC_RMID, NULL);
            semctl(mutex_handle_, IPC_RMID, 0);
        }
        data_ = NULL;
#else
        UnmapViewOfFile(data_);
        if (shmem_handle_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(shmem_handle_);
            shmem_handle_ = INVALID_HANDLE_VALUE;
        }
        CloseHandle(mutex_handle_);
        data_ = NULL;
#endif
    }

    Handle mutex() const { return mutex_handle_; }

    template<typename T> 
    T* get() { return (T*)data_; };

    template<typename T>
    bool try_write(const T* data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;

        std::memcpy(data_, data, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }

    template<typename T>
    bool try_read(T* data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;
        std::memcpy(data, data_, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }

    template<typename T>
    bool try_write(const T& data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;
        std::memcpy(data_, &data, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }

    template<typename T>
    bool try_read(T& data, const int& timeout_msec = 0)
    {
        if(!wait_for_single_object(mutex_handle_, timeout_msec))
            return false;
        std::memcpy(&data, data_, buffer_size_);
        release_mutex(mutex_handle_);
        return true;
    }

    bool wait_for_single_object(const Handle& mutex_handle, const int& timeout_msec = 0) const
    {
#ifdef __unix__
        using std::chrono::milliseconds;
        using std::chrono::system_clock;
        struct sembuf sop;
        sop.sem_num =  0;    
        sop.sem_op  = -1;
        sop.sem_flg = (timeout_msec>0) ? IPC_NOWAIT : 0;

        auto end_time = system_clock::now() + milliseconds(timeout_msec);
        
        while(true)
        {
            if(semop(mutex_handle, &sop, 1) == 0)
                return true;
            else if(errno != EAGAIN)
            {
                std::stringstream ss;
                ss << "semop lock failed. error code : " << errno << std::endl;
                throw std::runtime_error(ss.str());
            }

            if(system_clock::now() > end_time)
                return false;
        }     
#else
        int timeout = (timeout_msec>0) ? timeout_msec : INFINITE;
        if(WaitForSingleObject(mutex_handle, timeout) == WAIT_OBJECT_0)
            return true;
        else
            return false;
#endif
    }

    void release_mutex(const Handle& mutex_handle) const
    {
#ifdef __unix__
        struct sembuf sop;
        sop.sem_num =  0;    
        sop.sem_op  =  1;
        sop.sem_flg =  0;
        if(semop(mutex_handle, &sop, 1)==-1)
        {
            std::stringstream ss;
            ss << "semop unlock failed. error code : " << errno << std::endl;
            throw std::runtime_error(ss.str());
        }
#else
        ReleaseMutex(mutex_handle);     
#endif
    }
};

}

#endif

#endif