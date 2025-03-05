#ifndef XCONTAINER_H
#define XCONTAINER_H
#pragma once

// Common headers
#include <atomic>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <memory>

// Helpers
namespace xcontainer
{
    namespace details
    {
        // Helper to extract the first argument type
        template<typename T>
        struct first_arg_type;

        template<typename R, typename Arg1, typename... Args>
        struct first_arg_type<R(Arg1, Args...)>
        {
            using type = Arg1;
        };

        template<typename R, typename C, typename Arg1, typename... Args>
        struct first_arg_type<R(C::*)(Arg1, Args...)>
        {
            using type = Arg1;
        };

        template<typename R, typename C, typename Arg1, typename... Args>
        struct first_arg_type<R(C::*)(Arg1, Args...) const>
        {
            using type = Arg1;
        };

        // Concept to check if the first argument is const
        template<typename T>
        concept is_first_arg_const = std::is_const_v<std::remove_reference_t<typename first_arg_type<decltype(&T::operator())>::type>>;
    }
}

// Include all the containers
#include "xcontainer_lock.h"
#include "xcontainer_unordered_lockless_map.h"
#include "xcontainer_fixedsize_memory_pool.h"

#endif