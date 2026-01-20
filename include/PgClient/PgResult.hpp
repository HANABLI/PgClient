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
#include <Json/Json.hpp>
#include <string_view>
#include <libpq-fe.h>
#include <memory.h>
#include <memory>

namespace Postgresql
{
    enum class PgStatus
    {
        Empty,
        CommandOk,
        TuplesOk,
        CopyOut,
        CopyIn,
        BadResponse,
        NonFatalError,
        FatalError,
        Unknown
    };
    class PgResult
    {
        // Lyfe Cycle Managment
    public:
        ~PgResult() noexcept;
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

        PgStatus Status() const;

        std::string_view View(int row, int col) const;

        std::string_view View(int row, const char* colName, std::string_view def = {}) const;

        std::string Text(int row, const char* colName, std::string_view def = {}) const;

        std::string TextRequired(int row, const char* colName) const;

        int Int(int row, const char* colName, int def) const;

        bool Bool(int row, const char* colName, bool def) const;

        Json::Value Json(int row, const char* colName, Json::Value::Type type) const;

        SystemUtils::DiagnosticsSender::UnsubscribeDelegate SubscribeToDiagnostics(
            SystemUtils::DiagnosticsSender::DiagnosticMessageDelegate delegate,
            size_t minLevel = 0);

    private:
        struct Impl;

        std::unique_ptr<Impl> impl_;
    };
}  // namespace Postgresql

#endif /* POSTGRE_SQL_PG_RESULT_HPP */