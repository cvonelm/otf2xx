/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2016, Technische Universität Dresden, Germany
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef INCLUDE_OTF2XX_DEFINITIONS_REF_COUNTED_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_REF_COUNTED_HPP

#include <atomic>

namespace otf2
{
    template <class T>
    class intrusive_ptr;
namespace definition
{
    namespace detail
    {
        class ref_counted
        {
        public:
            ref_counted(int64_t retain_count = 0) : ref_count_(retain_count)
            {
            }

            ref_counted(const ref_counted&) = delete;
            ref_counted& operator=(const ref_counted&) = delete;

            ref_counted(ref_counted&&) = delete;
            ref_counted& operator=(ref_counted&&) = delete;

        private:

            void retain()
            {
                ref_count_.fetch_add(1, std::memory_order_relaxed);
            }

            int64_t release()
            {
                // fetch_sub() returns the old value, preceeding the operation.
                // Thus, we have to subtract one from the return value.
                return ref_count_.fetch_sub(1, std::memory_order_acq_rel) - 1;
            }

            template<class T>
            friend class ::otf2::intrusive_ptr;

            std::atomic<int64_t> ref_count_;
        };
    }
}
}

#endif // INCLUDE_OTF2XX_DEFINITIONS_REF_COUNTED_HPP