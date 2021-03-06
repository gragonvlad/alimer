//
// Copyright (c) 2018 Amer Koleci and contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "AlimerConfig.h"
#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>

namespace Alimer
{
    class RefCounted;
    template <class T> class WeakPtr;

    /// Reference count structure.
    struct RefCount
    {
        /// Construct.
        RefCount() = default;

        /// Destruct.
        ~RefCount()
        {
            refs = -1;
            weakRefs = -1;
        }

        int refs{};
        /// Weak reference count.
        int weakRefs{};
    };

    /// Base class for intrusively reference counted objects that can be pointed to with SharedPtr and WeakPtr. These are not copy-constructible and not assignable.
    class ALIMER_API RefCounted
    {
    public:
        /// Construct. The reference count is not allocated yet; it will be allocated on demand.
        RefCounted();

        /// Destruct. If no weak references, destroy also the reference count, else mark it expired.
        virtual ~RefCounted();

        /// Prevent copy construction.
        RefCounted(const RefCounted& rhs) = delete;
        /// Prevent assignment.
        RefCounted& operator =(const RefCounted& rhs) = delete;

        /// Add a strong reference. 
        void AddRef();
        /// Release a strong reference. 
        void Release();

        /// Return the number of strong references.
        int Refs() const;
        /// Return the number of weak references.
        int WeakRefs() const;
        /// Return pointer to the reference count structure. Allocate if not allocated yet.
        RefCount* RefCountPtr() { return _refCount; }

    private:
        /// Reference count structure, allocated on demand.
        RefCount* _refCount;
    };

    /// Shared pointer template class with intrusive reference counting.
    template <class T> class SharedPtr
    {
    public:
        /// Construct a null shared pointer.
        SharedPtr() noexcept : ptr_(nullptr)
        {
        }

        /// Construct a null shared pointer.
        SharedPtr(std::nullptr_t) noexcept // NOLINT(google-explicit-constructor)
            : ptr_(nullptr)
        {
        }

        /// Copy-construct from another shared pointer.
        SharedPtr(const SharedPtr<T>& rhs) noexcept
            : ptr_(rhs.ptr_)
        {
            InternalAddRef();
        }

        /// Copy-construct from another shared pointer allowing implicit upcasting.
        template <class U> SharedPtr(const SharedPtr<U>& rhs) noexcept  // NOLINT(google-explicit-constructor)
            : ptr_(rhs.ptr_)
        {
            InternalAddRef();
        }

        /// Construct from a raw pointer.
        explicit SharedPtr(T* ptr) noexcept
            : ptr_(ptr)
        {
            InternalAddRef();
        }

        /// Destruct. Release the object reference.
        ~SharedPtr() noexcept
        {
            InternalRelease();
        }

        /// Assign from another shared pointer.
        SharedPtr<T>& operator =(const SharedPtr<T>& rhs)
        {
            if (ptr_ == rhs.ptr_)
                return *this;

            SharedPtr<T> copy(rhs);
            Swap(copy);

            return *this;
        }

        /// Assign from another shared pointer allowing implicit upcasting.
        template <class U> SharedPtr<T>& operator =(const SharedPtr<U>& rhs)
        {
            if (ptr_ == rhs.ptr_)
                return *this;

            SharedPtr<T> copy(rhs);
            Swap(copy);

            return *this;
        }

        /// Assign from a raw pointer.
        SharedPtr<T>& operator =(T* ptr)
        {
            if (ptr_ == ptr)
                return *this;

            SharedPtr<T> copy(ptr);
            Swap(copy);

            return *this;
        }

        /// Point to the object.
        T* operator ->() const
        {
            assert(ptr_);
            return ptr_;
        }

        /// Dereference the object.
        T& operator *() const
        {
            assert(ptr_);
            return *ptr_;
        }

        /// Subscript the object if applicable.
        T& operator [](size_t index)
        {
            assert(ptr_);
            return ptr_[index];
        }

        /// Test for less than with another shared pointer.
        template <class U> bool operator <(const SharedPtr<U>& rhs) const { return ptr_ < rhs.ptr_; }

        /// Test for equality with another shared pointer.
        template <class U> bool operator ==(const SharedPtr<U>& rhs) const { return ptr_ == rhs.ptr_; }

        /// Test for inequality with another shared pointer.
        template <class U> bool operator !=(const SharedPtr<U>& rhs) const { return ptr_ != rhs.ptr_; }

        /// Convert to a raw pointer.
        operator T*() const { return ptr_; }    // NOLINT(google-explicit-constructor)

                                                /// Swap with another SharedPtr.
        void Swap(SharedPtr& rhs) { std::swap(ptr_, rhs.ptr_); }

        /// Reset to null and release the object reference.
        void Reset() { InternalRelease(); }

        /// Detach without destroying the object even if the refcount goes zero. To be used for scripting language interoperation.
        T* Detach()
        {
            T* ptr = ptr_;
            if (ptr_)
            {
                RefCount* refCount = RefCountPtr();
                ++refCount->refs; // 2 refs
                Reset(); // 1 ref
                --refCount->refs; // 0 refs
            }
            return ptr;
        }

        /// Perform a static cast from a shared pointer of another type.
        template <class U> void StaticCast(const SharedPtr<U>& rhs)
        {
            SharedPtr<T> copy(static_cast<T*>(rhs.Get()));
            Swap(copy);
        }

        /// Perform a dynamic cast from a shared pointer of another type.
        template <class U> void DynamicCast(const SharedPtr<U>& rhs)
        {
            SharedPtr<T> copy(dynamic_cast<T*>(rhs.Get()));
            Swap(copy);
        }

        /// Check if the pointer is null.
        bool IsNull() const { return ptr_ == nullptr; }

        /// Check if the pointer is not null.
        bool IsNotNull() const { return ptr_ != nullptr; }

        /// Return the raw pointer.
        T* Get() const { return ptr_; }

        /// Return the object's reference count, or 0 if the pointer is null.
        int Refs() const { return ptr_ ? ptr_->Refs() : 0; }

        /// Return the object's weak reference count, or 0 if the pointer is null.
        int WeakRefs() const { return ptr_ ? ptr_->WeakRefs() : 0; }

        /// Return pointer to the RefCount structure.
        RefCount* RefCountPtr() const { return ptr_ ? ptr_->RefCountPtr() : nullptr; }

        /// Return hash value for HashSet & HashMap.
        size_t ToHash() const { return (size_t)ptr_ / sizeof(T); }

    private:
        template <class U> friend class SharedPtr;

        /// Add a reference to the object pointed to.
        void InternalAddRef()
        {
            if (ptr_ != nullptr)
            {
                ptr_->AddRef();
            }
        }

        /// Release the object reference and delete it if necessary.
        void InternalRelease()
        {
            if (ptr_ != nullptr)
            {
                ptr_->Release();
                ptr_ = nullptr;
            }
        }

        /// Pointer to the object.
        T* ptr_;
    };

    /// Perform a static cast from one shared pointer type to another.
    template <class T, class U> SharedPtr<T> StaticCast(const SharedPtr<U>& ptr)
    {
        SharedPtr<T> ret;
        ret.StaticCast(ptr);
        return ret;
    }

    /// Perform a dynamic cast from one weak pointer type to another.
    template <class T, class U> SharedPtr<T> DynamicCast(const SharedPtr<U>& ptr)
    {
        SharedPtr<T> ret;
        ret.DynamicCast(ptr);
        return ret;
    }

    /// Weak pointer template class with intrusive reference counting. Does not keep the object pointed to alive.
    template <class T> class WeakPtr
    {
    public:
        /// Construct a null weak pointer.
        WeakPtr() noexcept : ptr_(nullptr), _refCount(nullptr)
        {
        }

        /// Construct a null weak pointer.
        WeakPtr(std::nullptr_t) noexcept  // NOLINT(google-explicit-constructor)
            : ptr_(nullptr)
            , _refCount(nullptr)
        {
        }

        /// Copy-construct from another weak pointer.
        WeakPtr(const WeakPtr<T>& rhs) noexcept
            : ptr_(rhs.ptr_)
            , _refCount(rhs._refCount)
        {
            AddRef();
        }

        /// Copy-construct from another weak pointer allowing implicit upcasting.
        template <class U> WeakPtr(const WeakPtr<U>& rhs) noexcept   // NOLINT(google-explicit-constructor)
            : ptr_(rhs.ptr_)
            , _refCount(rhs._refCount)
        {
            AddRef();
        }

        /// Construct from a shared pointer.
        WeakPtr(const SharedPtr<T>& rhs) noexcept // NOLINT(google-explicit-constructor)
            : ptr_(rhs.Get())
            , _refCount(rhs.RefCountPtr())
        {
            AddRef();
        }

        /// Construct from a raw pointer.
        explicit WeakPtr(T* ptr) noexcept
            : ptr_(ptr)
            , _refCount(ptr ? ptr->RefCountPtr() : nullptr)
        {
            AddRef();
        }

        /// Destruct. Release the weak reference to the object.
        ~WeakPtr() noexcept
        {
            ReleaseRef();
        }

        /// Assign from a shared pointer.
        WeakPtr<T>& operator =(const SharedPtr<T>& rhs)
        {
            if (ptr_ == rhs.Get() && _refCount == rhs.RefCountPtr())
                return *this;

            ReleaseRef();
            ptr_ = rhs.Get();
            _refCount = rhs.RefCountPtr();
            AddRef();

            return *this;
        }

        /// Assign from a weak pointer.
        WeakPtr<T>& operator =(const WeakPtr<T>& rhs)
        {
            if (ptr_ == rhs.ptr_ && _refCount == rhs._refCount)
                return *this;

            ReleaseRef();
            ptr_ = rhs.ptr_;
            _refCount = rhs._refCount;
            AddRef();

            return *this;
        }

        /// Assign from another weak pointer allowing implicit upcasting.
        template <class U> WeakPtr<T>& operator =(const WeakPtr<U>& rhs)
        {
            if (ptr_ == rhs.ptr_ && _refCount == rhs._refCount)
                return *this;

            ReleaseRef();
            ptr_ = rhs.ptr_;
            _refCount = rhs._refCount;
            AddRef();

            return *this;
        }

        /// Assign from a raw pointer.
        WeakPtr<T>& operator =(T* ptr)
        {
            RefCount* refCount = ptr ? ptr->RefCountPtr() : nullptr;

            if (ptr_ == ptr && _refCount == refCount)
                return *this;

            ReleaseRef();
            ptr_ = ptr;
            _refCount = refCount;
            AddRef();

            return *this;
        }

        /// Convert to a shared pointer. If expired, return a null shared pointer.
        SharedPtr<T> Lock() const
        {
            if (!IsExpired())
                return SharedPtr<T>(ptr_);

            return SharedPtr<T>();
        }

        /// Return raw pointer. If expired, return null.
        T* Get() const
        {
            return IsExpired() ? nullptr : ptr_;
        }

        /// Point to the object.
        T* operator ->() const
        {
            T* rawPtr = Get();
            assert(rawPtr);
            return rawPtr;
        }

        /// Dereference the object.
        T& operator *() const
        {
            T* rawPtr = Get();
            assert(rawPtr);
            return *rawPtr;
        }

        /// Subscript the object if applicable.
        T& operator [](size_t index)
        {
            T* rawPtr = Get();
            assert(rawPtr);
            return (*rawPtr)[index];
        }

        /// Test for equality with another weak pointer.
        template <class U> bool operator ==(const WeakPtr<U>& rhs) const { return ptr_ == rhs.ptr_ && _refCount == rhs._refCount; }

        /// Test for inequality with another weak pointer.
        template <class U> bool operator !=(const WeakPtr<U>& rhs) const { return ptr_ != rhs.ptr_ || _refCount != rhs._refCount; }

        /// Test for less than with another weak pointer.
        template <class U> bool operator <(const WeakPtr<U>& rhs) const { return ptr_ < rhs.ptr_; }

        /// Convert to a raw pointer, null if the object is expired.
        operator T*() const { return Get(); }   // NOLINT(google-explicit-constructor)

                                                /// Reset to null and release the weak reference.
        void Reset() { ReleaseRef(); }

        /// Perform a static cast from a weak pointer of another type.
        template <class U> void StaticCast(const WeakPtr<U>& rhs)
        {
            ReleaseRef();
            ptr_ = static_cast<T*>(rhs.Get());
            _refCount = rhs._refCount;
            AddRef();
        }

        /// Perform a dynamic cast from a weak pointer of another type.
        template <class U> void DynamicCast(const WeakPtr<U>& rhs)
        {
            ReleaseRef();
            ptr_ = dynamic_cast<T*>(rhs.Get());

            if (ptr_)
            {
                _refCount = rhs._refCount;
                AddRef();
            }
            else
                _refCount = nullptr;
        }

        /// Check if the pointer is null.
        bool IsNull() const { return _refCount == nullptr; }

        /// Check if the pointer is not null.
        bool IsNotNull() const { return _refCount != nullptr; }

        /// Return the object's reference count, or 0 if null pointer or if object has expired.
        int Refs() const { return (_refCount && _refCount->refs >= 0) ? _refCount->refs : 0; }

        /// Return the object's weak reference count.
        int WeakRefs() const
        {
            if (!IsExpired())
                return ptr_->WeakRefs();

            return _refCount ? _refCount->weakRefs : 0;
        }

        /// Return whether the object has expired. If null pointer, always return true.
        bool IsExpired() const { return _refCount ? _refCount->refs < 0 : true; }

        /// Return pointer to the RefCount structure.
        RefCount* RefCountPtr() const { return _refCount; }

        /// Return hash value for HashSet & HashMap.
        size_t ToHash() const { return (size_t)ptr_ / sizeof(T); }

    private:
        template <class U> friend class WeakPtr;

        /// Add a weak reference to the object pointed to.
        void AddRef()
        {
            if (_refCount)
            {
                assert(_refCount->weakRefs >= 0);
                ++(_refCount->weakRefs);
            }
        }

        /// Release the weak reference. Delete the Refcount structure if necessary.
        void ReleaseRef()
        {
            if (_refCount)
            {
                assert(_refCount->weakRefs > 0);
                --(_refCount->weakRefs);

                if (IsExpired() && !_refCount->weakRefs)
                    delete _refCount;
            }

            ptr_ = nullptr;
            _refCount = nullptr;
        }

        /// Pointer to the object.
        T* ptr_;
        /// Pointer to the RefCount structure.
        RefCount* _refCount;
    };

    /// Perform a static cast from one weak pointer type to another.
    template <class T, class U> WeakPtr<T> StaticCast(const WeakPtr<U>& ptr)
    {
        WeakPtr<T> ret;
        ret.StaticCast(ptr);
        return ret;
    }

    /// Perform a dynamic cast from one weak pointer type to another.
    template <class T, class U> WeakPtr<T> DynamicCast(const WeakPtr<U>& ptr)
    {
        WeakPtr<T> ret;
        ret.DynamicCast(ptr);
        return ret;
    }

    /// Delete object of type T. T must be complete. See boost::checked_delete.
    template<class T> inline void CheckedDelete(T* x)
    {
        // intentionally complex - simplification causes regressions
        using type_must_be_complete = char[sizeof(T) ? 1 : -1];
        (void) sizeof(type_must_be_complete);
        delete x;
    }

    /// Unique pointer template class.
    template <class T> class UniquePtr
    {
    public:
        /// Construct empty.
        UniquePtr() : _ptr(nullptr) { }

        /// Construct from pointer.
        explicit UniquePtr(T* ptr) : _ptr(ptr) { }

        /// Prevent copy construction.
        UniquePtr(const UniquePtr&) = delete;
        /// Prevent assignment.
        UniquePtr& operator=(const UniquePtr&) = delete;

        /// Assign from pointer.
        UniquePtr& operator = (T* ptr)
        {
            Reset(ptr);
            return *this;
        }

        /// Construct empty.
        UniquePtr(std::nullptr_t) { }   // NOLINT(google-explicit-constructor)


                                        /// Move-construct from UniquePtr.
        UniquePtr(UniquePtr&& up) noexcept : _ptr(up.Detach()) {}

        /// Move-assign from UniquePtr.
        UniquePtr& operator =(UniquePtr&& up) noexcept
        {
            Reset(up.Detach());
            return *this;
        }

        /// Point to the object.
        T* operator ->() const
        {
            assert(_ptr);
            return _ptr;
        }

        /// Dereference the object.
        T& operator *() const
        {
            assert(_ptr);
            return *_ptr;
        }

        /// Test for less than with another unique pointer.
        template <class U>
        bool operator <(const UniquePtr<U>& rhs) const { return _ptr < rhs._ptr; }

        /// Test for equality with another unique pointer.
        template <class U>
        bool operator ==(const UniquePtr<U>& rhs) const { return _ptr == rhs._ptr; }

        /// Test for inequality with another unique pointer.
        template <class U>
        bool operator !=(const UniquePtr<U>& rhs) const { return _ptr != rhs._ptr; }

        /// Cast pointer to bool.
        operator bool() const { return !!_ptr; }    // NOLINT(google-explicit-constructor)

                                                    /// Swap with another UniquePtr.
        void Swap(UniquePtr& up) { std::swap(_ptr, up._ptr); }

        /// Detach pointer from UniquePtr without destroying.
        T* Detach()
        {
            T* ptr = _ptr;
            _ptr = nullptr;
            return ptr;
        }

        /// Check if the pointer is null.
        bool IsNull() const { return _ptr == nullptr; }

        /// Check if the pointer is not null.
        bool IsNotNull() const { return _ptr != nullptr; }

        /// Return the raw pointer.
        T* Get() const { return _ptr; }

        /// Reset.
        void Reset(T* ptr = nullptr)
        {
            CheckedDelete(_ptr);
            _ptr = ptr;
        }

        /// Return hash value for HashSet & HashMap.
        size_t ToHash() const { return (size_t)_ptr / sizeof(T); }

        /// Destruct.
        ~UniquePtr()
        {
            Reset();
        }

    private:
        T * _ptr;
    };

    /// Pointer which takes ownership of an array allocated with new[] and deletes it when the pointer goes out of scope.
    template <class T> class AutoArrayPtr
    {
    public:
        /// Construct a null pointer.
        AutoArrayPtr() :  _array(nullptr)
        {
        }

        /// Copy-construct. Ownership is transferred, making the source pointer null.
        AutoArrayPtr(AutoArrayPtr<T>& ptr) : _array(ptr._array)
        {
            ptr._array = nullptr;
        }

        /// Construct and take ownership of the array.
        AutoArrayPtr(T* array_) : _array(array_)
        {
        }

        /// Destruct. Delete the array pointed to.
        ~AutoArrayPtr()
        {
            delete[] _array;
        }

        /// Assign from a pointer. Existing array is deleted and ownership is transferred from the source pointer, which becomes null.
        AutoArrayPtr<T>& operator = (AutoArrayPtr<T>& rhs)
        {
            delete _array;
            _array = rhs._array;
            rhs._array = nullptr;
            return *this;
        }

        /// Assign a new array. Existing array is deleted.
        AutoArrayPtr<T>& operator = (T* rhs)
        {
            delete _array;
            _array = rhs;
            return *this;
        }

        /// Detach the array from the pointer without destroying it and return it. The pointer becomes null.
        T* Detach()
        {
            T* ret = _array;
            _array = nullptr;
            return ret;
        }

        /// Reset to null. Destroys the array.
        void Reset()
        {
            *this = nullptr;
        }

        /// Point to the array.
        T* operator -> () const { assert(_array); return _array; }
        /// Dereference the array.
        T& operator * () const { assert(_array); return *_array; }
        /// Index the array.
        T& operator [] (size_t index) { assert(_array); return _array[index]; }
        /// Const-index the array.
        const T& operator [] (size_t index) const { assert(_array); return _array[index]; }
        /// Convert to bool.
        operator bool() const { return _array != nullptr; }

        /// Return the array.
        T* Get() const { return _array; }

        /// Return whether is a null pointer.
        bool IsNull() const { return _array == nullptr; }

        /// Check if the pointer is not null.
        bool IsNotNull() const { return _array != nullptr; }

    private:
        /// Array pointer.
        T* _array;
    };

    /// Swap two UniquePtr-s.
    template <class T> void Swap(UniquePtr<T>& first, UniquePtr<T>& second)
    {
        first.Swap(second);
    }

    /// Construct UniquePtr.
    template <class T, class ... Args> UniquePtr<T> MakeUnique(Args && ... args)
    {
        return UniquePtr<T>(new T(std::forward<Args>(args)...));
    }

    /// Construct SharedPtr.
    template <class T, class ... Args> SharedPtr<T> MakeShared(Args && ... args)
    {
        return SharedPtr<T>(new T(std::forward<Args>(args)...));
    }
}
