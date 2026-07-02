// SPDX-FileCopyrightText: © 2026 Tenstorrent USA, Inc.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "mem.h"
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <cassert>

class sparse_mem : public mem {

    public:

        sparse_mem(addr_t page_size = 1);

        using mem::read;
        using mem::write;
        using mem::check;

        bool read (addr_t addr, sz_t size,       datum_t* data                                                              )       override;
        void write(addr_t addr, sz_t size, const datum_t* data, const mem::write_options* opt =  &mem::write_options_default)       override;
        bool check(addr_t addr, sz_t size, const datum_t* data, bool allow_unininitialized = false                          ) const override;

    private:

        std::unordered_map<addr_t, data_t > pages_;
        std::unordered_map<addr_t, datum_t> unpaged_; // for small page sizes

        bool unpaged() const { return page_size_ <= 1; }

        template <typename T, bool CREATE>
            static auto get_or_optionally_init_page(T& t, addr_t a, const datum_t* init_data = nullptr){

                using P = std::conditional_t<std::is_const_v<T>, const datum_t*, datum_t*>;
                bool created = false;

                if (t.unpaged()) {
                    auto it = t.unpaged_.find(a);
                    if (it == t.unpaged_.end()) {
                        if constexpr (!CREATE) {
                            return std::make_tuple(P(nullptr), P(nullptr), false);
                        } else {
                            it = t.unpaged_.emplace(a, init_data ? *init_data : t.uninitialized_read(a, 1)[0]).first;
                        }
                        created = true;
                    }
                    return std::make_tuple(&(it->second), &(it->second)+1, CREATE ? created : true);
                }

                addr_t page_no     = a >> t.page_shift_;
                addr_t page_offset = a & (t.page_size_ - 1);

                auto it = t.pages_.find(page_no);

                if (it == t.pages_.end()) {
                    if constexpr (!CREATE) {
                        return std::make_tuple(P(nullptr), P(nullptr), false);
                    } else {
                        if (init_data) {
                            assert(page_offset == 0);
                            it = t.pages_.emplace(
                                    std::piecewise_construct,
                                    std::forward_as_tuple(page_no),
                                    std::forward_as_tuple(init_data, init_data + t.page_size_)
                                    ).first;
                        } else {
                            it = t.pages_.emplace(page_no, t.uninitialized_read(a & ~(t.page_size_ - 1), t.page_size_)).first;
                        }
                    }
                    created = true;
                }

                auto begin = &(*std::next(it->second.begin(), page_offset));
                auto end   = &(it->second.back())+1;
                return std::make_tuple(begin, end, CREATE ? created : true);
            }

        std::tuple<datum_t*, datum_t*, bool> get_or_init_page(addr_t a);
        std::tuple<datum_t*, datum_t*, bool> get_or_emplace_page(addr_t a, const datum_t* data);
        std::tuple<const datum_t*, const datum_t*, bool> get_page(addr_t a) const;
        std::tuple<datum_t*, datum_t*, bool> get_page(addr_t a);

        addr_t page_shift_;
        addr_t page_size_;

};
