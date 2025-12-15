/**
 * @file PgClient.cpp
 *
 * @brief this module contains the implementation of the PgClient
 * class.
 *
 * @copyright © copyright 2025 by Hatem Nabli.
 */

#include <PgClient/PgClient.hpp>

namespace Postgresql
{
    struct PgClient::Impl
    {
        PGconn* conn = nullptr;
        std::string lastError;
        /**
         * This is a helper object used to generate and publish
         * diagnostic message.
         */
        SystemUtils::DiagnosticsSender diagnosticsSender;
        Impl() : diagnosticsSender("Postgresql::PgClient") {}
        ~Impl() {
            if (conn)
            { PQfinish(conn); }
        }
    };

    PgClient::PgClient() : impl_(std::make_unique<Impl>()) {}

    /**
     * This is the connect method.
     */
    bool PgClient::Connect(const std::string& conninfo) {
        impl_->conn = PQconnectdb(conninfo.c_str());
        if (PQstatus(impl_->conn) != CONNECTION_OK)
        {
            impl_->lastError = PQerrorMessage(impl_->conn);
            PQfinish(impl_->conn);
            impl_->conn = nullptr;
            return false;
        }
        return true;
    }

    /**
     * This method get the last occurred error.
     */
    const std::string& PgClient::LastError() const { return impl_->lastError; }

    /**
     * This method exacute a simple request without parameters
     */
    PGresult* PgClient::Exec(const std::string& sql) {
        PGresult* res = PQexec(impl_->conn, sql.c_str());
        if (!res)
        {
            impl_->lastError = PQerrorMessage(impl_->conn);
            return nullptr;
        }
        auto status = PQresultStatus(res);
        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
        {
            impl_->lastError = PQerrorMessage(impl_->conn);
            PQclear(res);
            return nullptr;
        }
        return res;
    }

    /**
     * This method exacute a request with the given parameters
     */
    PGresult* PgClient::ExecParams(const std::string& sql, const std::vector<std::string>& params) {
        std::vector<const char*> values;
        values.reserve(params.size());
        for (auto& p : params)
        { values.push_back(p.c_str()); }

        PGresult* res = PQexecParams(impl_->conn, sql.c_str(), (int)params.size(), nullptr,
                                     values.data(), nullptr, nullptr, 0);
        if (!res)
        {
            impl_->lastError = PQerrorMessage(impl_->conn);
            return nullptr;
        }

        auto status = PQresultStatus(res);
        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
        {
            impl_->lastError = PQerrorMessage(impl_->conn);
            PQclear(res);
            return nullptr;
        }
        return res;
    }

    SystemUtils::DiagnosticsSender::UnsubscribeDelegate PgClient::SubscribeToDiagnostics(
        SystemUtils::DiagnosticsSender::DiagnosticMessageDelegate delegate, size_t minLevel) {
        return impl_->diagnosticsSender.SubscribeToDiagnostics(delegate, minLevel);
    }
}  // namespace Postgresql