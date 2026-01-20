#ifndef POSTGRE_SQL_PG_CLIENT_HPP
#define POSTGRE_SQL_PG_CLIENT_HPP
/**
 * @file PgClient.hpp
 *
 * @brief This is the definition of the PgClient Class, it implement the C libpq
 * official postgreSQL Api.
 *
 * @copyright © copyright 2025 by Hatem Nabli.
 */
#include <SystemUtils/DiagnosticsSender.hpp>

#include <libpq-fe.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <functional>

namespace Postgresql
{
    class PgClient
    {  // Life cycle managment

    public:
        ~PgClient() = default;
        PgClient(const PgClient&) = delete;
        PgClient(PgClient&&) noexcept = default;
        PgClient& operator=(const PgClient&) = delete;
        PgClient& operator=(PgClient&&) noexcept = default;
        // Methods
    public:
        PgClient();

        /**
         * This is the connect method.
         */
        bool Connect(const std::string& conninfo);

        /**
         * This method get the last occurred error.
         */
        const std::string& LastError() const;

        /**
         * This method exacute a simple request without parameters
         */
        PGresult* Exec(const std::string& sql);

        /**
         * This method exacute a request with the given parameters
         */
        PGresult* ExecParams(const std::string& sql, const std::vector<std::string>& params);

        /**
         * This method start a listener to the given channel.
         */
        bool Listen(const std::string& channel);

        bool Listen(const std::string& channel, const std::function<void()>& fn);

        /**
         * This method return a payload when notify is available
         */
        bool PollNotify(std::string& outChannel, std::string& outPayload);

        /**
         * This method wait for the given timeout value and then attempt pollnotify
         *
         */
        bool WaitNotify(std::string& outChannel, std::string& outPayload, int timeoutMs);

        SystemUtils::DiagnosticsSender::UnsubscribeDelegate SubscribeToDiagnostics(
            SystemUtils::DiagnosticsSender::DiagnosticMessageDelegate delegate,
            size_t minLevel = 0);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
    };
}  // namespace Postgresql

#endif /* POSTGRE_SQL_PG_CLIENT_HPP */
