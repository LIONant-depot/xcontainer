#ifndef XCONTAINER_SMALL_VECTOR_H
#define XCONTAINER_SMALL_VECTOR_H
#pragma once

namespace xcontainer
{
    template<typename T, std::size_t T_INLINE_COUNT = 4>
    class small_vector
    {
    public:

        using iterator               = T*;
        using const_iterator         = const T*;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // Constructors
        small_vector() noexcept = default;

        // Copy constructor
        small_vector(const small_vector& other)
        {
            reserve(other.m_Size);
            std::uninitialized_copy(other.m_pData, other.m_pData + other.m_Size, m_pData);
            m_Size = other.m_Size;
        }

        // Move constructor
        small_vector(small_vector&& other) noexcept
            : m_Size(other.m_Size)
            , m_Capacity(other.m_Capacity)
            // m_pData is left at its default member initializer (this object's OWN m_Inline) - the
            // heap-case branch below overwrites it. It must NOT be initialized from other.m_pData: when
            // other was using ITS OWN inline buffer, that would leave this->m_pData aliasing
            // other.m_Inline (a different object's storage) instead of this->m_Inline, and later - since
            // that address never equals this->m_Inline - the destructor would wrongly treat it as a heap
            // pointer and call _aligned_free() on stack/member memory (observed as the MSVC debug CRT
            // "was not allocated by _aligned routines" abort, always on whatever object destructs this
            // corrupted vector - e.g. at process exit for a long-lived container).
        {
            if (other.m_pData == reinterpret_cast<T*>(other.m_Inline))
            {
                // Move inline elements into THIS object's own inline buffer
                m_pData = reinterpret_cast<T*>(m_Inline);
                std::uninitialized_move(other.m_pData, other.m_pData + other.m_Size, m_pData);
                std::destroy(other.m_pData, other.m_pData + other.m_Size);
            }
            else
            {
                // Take ownership of heap buffer
                m_pData = other.m_pData;
            }

            other.m_Size = 0;
            other.m_Capacity = T_INLINE_COUNT;
            other.m_pData = reinterpret_cast<T*>(other.m_Inline);
        }

        // Destructor
        ~small_vector()
        {
            std::destroy(m_pData, m_pData + m_Size);

            if (m_pData != reinterpret_cast<T*>(m_Inline))
            {
                _aligned_free(m_pData);
            }
        }

        void reset()
        {
            clear();

            if (m_pData != reinterpret_cast<T*>(m_Inline))
            {
                _aligned_free(m_pData);
            }

            m_pData = reinterpret_cast<T*>(m_Inline);
            m_Capacity = T_INLINE_COUNT;
        }

        // Copy assignment
        small_vector& operator = (const small_vector& other)
        {
            if (this == &other) return *this;
            reset();
            reserve(other.m_Size);
            std::uninitialized_copy(other.m_pData, other.m_pData + other.m_Size, m_pData);
            m_Size = other.m_Size;
            return *this;
        }

        // Move assignment
        small_vector& operator = (small_vector&& other) noexcept
        {
            if (this == &other) return *this;
            reset();
            if (other.m_pData == reinterpret_cast<T*>(other.m_Inline))
            {
                std::uninitialized_move(other.m_pData, other.m_pData + other.m_Size, m_pData);
                std::destroy(other.m_pData, other.m_pData + other.m_Size);
                m_Size = other.m_Size;
            }
            else
            {
                m_pData    = other.m_pData;
                m_Size     = other.m_Size;
                m_Capacity = other.m_Capacity;
            }

            other.m_Size     = 0;
            other.m_Capacity = T_INLINE_COUNT;
            other.m_pData    = reinterpret_cast<T*>(other.m_Inline);
            return *this;
        }

        // Push back
        void push_back(const T& value)
        {
            if (m_Size >= m_Capacity)
            {
                grow();
            }
            new (m_pData + m_Size) T(value);
            ++m_Size;
        }

        void push_back(T&& value)
        {
            if (m_Size >= m_Capacity)
            {
                grow();
            }
            new (m_pData + m_Size) T(std::move(value));
            ++m_Size;
        }

        template<typename... Args>
        void emplace_back(Args&&... args)
        {
            if (m_Size >= m_Capacity)
            {
                grow();
            }

            new (m_pData + m_Size) T(std::forward<Args>(args)...);
            ++m_Size;
        }

        // Accessors
        T& operator[](size_t index) noexcept
        {
            assert(index < m_Size);
            return m_pData[index];
        }

        const T& operator[](size_t index) const noexcept
        {
            assert(index < m_Size);
            return m_pData[index];
        }

        T& at(size_t index)
        {
            if (index >= m_Size) throw std::out_of_range("Index out of bounds");
            return m_pData[index];
        }

        const T& at(size_t index) const
        {
            if (index >= m_Size) throw std::out_of_range("Index out of bounds");
            return m_pData[index];
        }

        // Size and capacity
        std::size_t size()      const noexcept { return m_Size; }
        std::size_t capacity()  const noexcept { return m_Capacity; }
        bool        empty()     const noexcept { return m_Size == 0; }

        void shrink_to_fit()
        {
            std::size_t new_capacity = std::max(m_Size, T_INLINE_COUNT);
            if (new_capacity < m_Capacity)
            {
                T* new_data = reinterpret_cast<T*>(_aligned_malloc(new_capacity * sizeof(T), alignof(T)));
                if (!new_data) throw std::bad_alloc();

                std::uninitialized_move(m_pData, m_pData + m_Size, new_data);
                std::destroy(m_pData, m_pData + m_Size);

                if (m_pData != reinterpret_cast<T*>(m_Inline))
                {
                    _aligned_free(m_pData);
                }

                m_pData = new_data;
                m_Capacity = new_capacity;
            }
        }
        // Modifiers
        void reserve(std::size_t new_capacity)
        {
            if (new_capacity > m_Capacity)
            {
                T* new_data = reinterpret_cast<T*>(_aligned_malloc(new_capacity * sizeof(T), alignof(T)));
                if (!new_data) throw std::bad_alloc();

                std::uninitialized_move(m_pData, m_pData + m_Size, new_data);
                std::destroy(m_pData, m_pData + m_Size);

                if (m_pData != reinterpret_cast<T*>(m_Inline))
                {
                    _aligned_free(m_pData);
                }

                m_pData = new_data;
                m_Capacity = new_capacity;
            }
        }

        // Resize
        void resize(std::size_t new_size)
        {
            if (new_size > m_Capacity)
            {
                reserve(std::max(new_size, T_INLINE_COUNT));
            }

            if (new_size > m_Size)
            {
                std::uninitialized_default_construct(m_pData + m_Size, m_pData + new_size);
            }
            else if (new_size < m_Size)
            {
                std::destroy(m_pData + new_size, m_pData + m_Size);
            }

            m_Size = new_size;
        }

        void resize(std::size_t new_size, const T& value)
        {
            if (new_size > m_Capacity)
            {
                reserve(std::max(new_size, T_INLINE_COUNT));
            }

            if (new_size > m_Size)
            {
                std::uninitialized_fill(m_pData + m_Size, m_pData + new_size, value);
            }
            else if (new_size < m_Size)
            {
                std::destroy(m_pData + new_size, m_pData + m_Size);
            }
            m_Size = new_size;
        }

        void clear() noexcept
        {
            std::destroy(m_pData, m_pData + m_Size);
            m_Size = 0;
        }

        // Iterators
        iterator        rbegin()            noexcept { return reverse_iterator(end()); }
        iterator        rend()              noexcept { return reverse_iterator(begin()); }
        const_iterator  rbegin()    const   noexcept { return const_reverse_iterator(end()); }
        const_iterator  rend()      const   noexcept { return const_reverse_iterator(begin()); }
        const_iterator  crbegin()   const   noexcept { return const_reverse_iterator(cend()); }
        const_iterator  crend()     const   noexcept { return const_reverse_iterator(cbegin()); }
        iterator        begin()             noexcept { return m_pData; }
        iterator        end()               noexcept { return m_pData + m_Size; }
        const_iterator  begin()     const   noexcept { return m_pData; }
        const_iterator  end()       const   noexcept { return m_pData + m_Size; }
        const_iterator  cbegin()    const   noexcept { return m_pData; }
        const_iterator  cend()      const   noexcept { return m_pData + m_Size; }

        // Swap - implemented via the (now-correct) move constructor/assignment rather than hand-rolled
        // pointer swapping: a mixed inline/heap case can't just swap m_pData values, since an inline
        // m_pData must end up pointing at ITS OWN object's m_Inline, not the other object's - the same
        // aliasing mistake the move constructor used to make (see its own comment).
        friend void swap(small_vector& a, small_vector& b) noexcept
        {
            small_vector tmp(std::move(a));
            a = std::move(b);
            b = std::move(tmp);
        }

    private:

        void grow()
        {
            std::size_t new_capacity = m_Capacity == 0 ? T_INLINE_COUNT : m_Capacity * 3 / 2; // 1.5x growth
            reserve(new_capacity);
        }

        std::size_t             m_Size = 0;
        std::size_t             m_Capacity = T_INLINE_COUNT;
        T* m_pData = reinterpret_cast<T*>(m_Inline);
        alignas(T) std::byte    m_Inline[sizeof(T) * T_INLINE_COUNT]{};
    };
}

#endif