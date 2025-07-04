/*
 * Copyright (C) 2024-present ScyllaDB
 */

/*
 * SPDX-License-Identifier: LicenseRef-ScyllaDB-Source-Available-1.0
 */

#include "cql3/description.hh"

#include <seastar/core/on_internal_error.hh>

#include "cql3/util.hh"
#include "utils/log.hh"
#include "types/types.hh"

static logging::logger dlogger{"description"};

namespace cql3 {

std::vector<managed_bytes_opt> description::serialize(bool serialize_create_statement) && {
    std::vector<managed_bytes_opt> result{};
    result.reserve(serialize_create_statement ? 4 : 3);

    if (keyspace) {
        result.push_back(to_managed_bytes(cql3::util::maybe_quote(*keyspace)));
    } else {
        result.push_back(to_managed_bytes_opt(data_value::make_null(utf8_type).serialize()));
    }

    result.push_back(to_managed_bytes(type));
    result.push_back(to_managed_bytes(cql3::util::maybe_quote(name)));

    if (serialize_create_statement && create_statement) {
        result.push_back(std::move(create_statement.value()).as_managed_bytes());
    } else if (serialize_create_statement) {
        on_internal_error(dlogger, "create_statement field is empty");
    }

    return result;
}

} // namespace cql3
