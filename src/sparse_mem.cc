#include "sparse_mem.h"
#include <iterator>
#include <iostream>

sparse_mem::sparse_mem(addr_t page_size) {

    if (!std::has_single_bit(page_size)) {
        throw std::invalid_argument("Page size must be a power of two");
    }

    page_size_ = page_size;
    page_shift_ = std::countr_zero(page_size);

}

bool sparse_mem::read(addr_t addr, sz_t size, datum_t* data) {

    bool addr_exists = true;

    for (sz_t s = 0; s < size;) {
        auto [page_it, page_end, created] = get_or_init_page(addr + s);
        if (created) addr_exists = false;
        sz_t page_len = std::distance(page_it, page_end);
        sz_t acc_len  = std::min(page_len, size - s);
        for (sz_t p = 0; p < acc_len; p++) {
            *data++ = *page_it++;
        }
        s += acc_len;
    }

    return addr_exists;
}

void sparse_mem::write(addr_t addr, sz_t size, const datum_t* data, const mem::write_options* opt) {

    if (unpaged()) {
        for (sz_t s = 0; s < size; s++, data++) {
            if (!opt->skip_zero || *data) {
                unpaged_[addr + s] = *data;
            }
        }

        return;
    }


    for (sz_t s = 0; s < size;) {
        if (opt->skip_zero) {
            auto it = std::find_if(data, data + size, [](const datum_t& d) { return d != 0; });
            auto diff = it - data;
            s += diff;
            if (!(s < size)) {
                return;
            }
            data += diff;
        }

        bool whole_page = ((addr + s) & (page_size_ - 1)) == 0 && (size - s) >= page_size_;
        auto [page_it, page_end, created] = whole_page ? get_or_emplace_page(addr + s, data) : get_or_init_page(addr + s);
        sz_t page_len = std::distance(page_it, page_end);
        sz_t acc_len  = std::min(page_len, size - s);
        if (!(whole_page && created)) {
            std::copy_n(data, acc_len, page_it);
        }
        s    += acc_len;
        data += acc_len;
    }

}

bool sparse_mem::check(addr_t addr, sz_t size, const datum_t* data, bool allow_uninitialized) const {

    for (sz_t s = 0; s < size;) {
        auto [page_it, page_end, valid] = get_page(addr + s);
        data_t uninitialized_data;
        if (!valid) {
            if (!allow_uninitialized) {
                return false;
            }
            uninitialized_data = uninitialized_read(addr + s, std::min(page_size_ - ((addr + s) & (page_size_-1)), size - s));
            page_it = &*uninitialized_data.begin();
            page_end = &uninitialized_data.back()+1;
        }
        sz_t page_len = std::distance(page_it, page_end);
        sz_t acc_len  = std::min(page_len, size - s);
        for (sz_t p = 0; p < acc_len; p++) {
            if(*data++ != *page_it++) {
                return false;
            }
        }
        s += acc_len;
    }

    return true;

}

std::tuple<mem::datum_t*, mem::datum_t*, bool> sparse_mem::get_or_init_page(addr_t a) {
    return get_or_optionally_init_page<sparse_mem, true>(*this, a);
}

std::tuple<mem::datum_t*, mem::datum_t*, bool> sparse_mem::get_or_emplace_page(addr_t a, const datum_t* data) {
    return get_or_optionally_init_page<sparse_mem, true>(*this, a, data);
}

std::tuple<const mem::datum_t*, const mem::datum_t*, bool> sparse_mem::get_page(addr_t a) const {
    return get_or_optionally_init_page<const sparse_mem, false>(*this, a);
}

std::tuple<mem::datum_t*, mem::datum_t*, bool> sparse_mem::get_page(addr_t a) {
    return get_or_optionally_init_page<sparse_mem, false>(*this, a);
}
