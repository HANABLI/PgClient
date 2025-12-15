#ifndef POSTGRE_SQL_PG_RESULT_HPP
#define POSTGRE_SQL_PG_RESULT_HPP
/**
 * @file PgResult.hpp
 *
 * @brief This is the definition of the PgResult wrapper,
 * This avoids all leaks and gives raw access..
 *
 * @copyright © copyright 2025 by Hatem Nabli.
 */
#include <SystemUtils/DiagnosticsSender.hpp>
#include <StringUtils/StringUtils.hpp>
#include <libpq-fe.h>
#include <memory.h>
#include <memory>

namespace Postgresql
{
    class PgResult
    {
        // Lyfe Cycle Managment
    public:
        ~PgResult() noexcept = default;
        PgResult(const PgResult&) = delete;
        PgResult(PgResult&&);
        PgResult& operator=(const PgResult&) = delete;
        PgResult& operator=(PgResult&&);

        // Methods
    public:
        explicit PgResult(PGresult*);

        PGresult* Raw() const;

        int Rows() const;

        int Cols() const;

        const char* Data(int raw, int col) const;

        int Length(int raw, int col) const;

        bool IsNull(int raw, int col) const;

        int ColIndex(const char* name) const;

        void Reset(PGresult* r = nullptr) noexcept;

        ExecStatusType Status() const;

        std::string_view View(int row, int col) const;

        SystemUtils::DiagnosticsSender::UnsubscribeDelegate SubscribeToDiagnostics(
            SystemUtils::DiagnosticsSender::DiagnosticMessageDelegate delegate,
            size_t minLevel = 0);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
    };
}  // namespace Postgresql

#endif /* POSTGRE_SQL_PG_RESULT_HPP */