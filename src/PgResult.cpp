/**
 *
 */

#include <PgClient/PgResult.hpp>
#include <memory>
#include <utility>
namespace Postgresql
{
    struct PgResult::Impl
    {
        /**
         * Here is the result obtained once the execution of a query is complete.
         */
        PGresult* result = nullptr;
        /**
         * This is a helper object used to generate and publish
         * diagnostic message.
         */
        SystemUtils::DiagnosticsSender diagnosticsSender;

        Impl() : diagnosticsSender("Postgresql::PgResult") {}
        ~Impl() {
            if (result)
                PQclear(result);
        }

        void Ensure() const {
            if (!result)
            { diagnosticsSender.SendDiagnosticInformationString(2, "PgResult: null PGresult*"); }
        }
    };

    PgResult::PgResult(PGresult* r) : impl_(std::make_unique<Impl>()) { impl_->result = r; }

    PgResult::PgResult(PgResult&& other) { impl_->result = std::move(other.Raw()); }
    PgResult::~PgResult() noexcept = default;

    PgResult& PgResult::operator=(PgResult&& other) {
        impl_->result = std::move(other.Raw());
        return *this;
    }

    PGresult* PgResult::Raw() const { return impl_->result; }

    int PgResult::Rows() const { return PQntuples(impl_->result); }

    int PgResult::Cols() const { return PQnfields(impl_->result); }

    const char* PgResult::Data(int row, int col) const {
        return PQgetvalue(impl_->result, row, col);
    }

    int PgResult::Length(int row, int col) const { return PQgetlength(impl_->result, row, col); }

    bool PgResult::IsNull(int row, int col) const {
        return PQgetisnull(impl_->result, row, col) != 0;
    }

    int PgResult::ColIndex(const char* name) const {
        impl_->Ensure();
        const int c = PQfnumber(impl_->result, name);
        if (c < 0)
        {
            impl_->diagnosticsSender.SendDiagnosticInformationFormatted(
                2, "PgResult: missing column %s", name);
        }
        return c;
    }

    void PgResult::Reset(PGresult* r) noexcept {
        if (impl_->result)
            PQclear(impl_->result);
        impl_->result = r;
    }

    ExecStatusType PgResult::Status() const {
        impl_->Ensure();
        return PQresultStatus(impl_->result);
    }

    std::string_view PgResult::View(int row, int col) const {
        if (IsNull(row, col))
            return {};
        return std::string_view{Data(row, col), static_cast<size_t>(Length(row, col))};
    }

    std::string_view PgResult::View(int row, const char* colName, std::string_view def) const {
        const int col = ColIndex(colName);
        if (IsNull(row, col))
            return def;
        return {Data(row, col), static_cast<size_t>(Length(row, col))};
    }

    std::string PgResult::Text(int row, const char* colName, std::string_view def) const {
        auto v = View(row, colName, def);
        return std::string(v);
    }

    std::string PgResult::TextRequired(int row, const char* colName) const {
        const int col = ColIndex(colName);
        if (IsNull(row, col))
            throw std::runtime_error(std::string("PgResult: required column'") + colName +
                                     "'is Null");
        auto v = View(row, col);
        return std::string(v);
    }

    int PgResult::Int(int row, const char* colName, int def) const {
        auto v = View(row, colName);
        if (v.empty())
            return def;

        char* end = nullptr;
        long l = std::strtol(v.data(), &end, 10);
        if (!end)
            throw std::runtime_error(std::string("PgResult: invalid int in column'") + colName +
                                     "'");
        return static_cast<int>(l);
    }

    bool PgResult::Bool(int row, const char* colName, bool def) const {
        auto v = View(row, colName);
        if (v.empty())
            return def;
        if (v == "t" || v == "true" || v == "1")
            return true;
        if (v == "f" || v == "false" || v == "0")
            return false;
        throw std::runtime_error(std::string("PgResult: invalid bool in '") + colName + "'");
    }

    Json::Value PgResult::Json(int row, const char* colName, Json::Value::Type type) const {
        const int col = ColIndex(colName);
        if (IsNull(row, col))
            throw std::runtime_error(std::string("PgResult: required column'") + colName +
                                     "'is Null");
        auto v = View(row, col);
        Json::Value j(type);
        j.FromEncoding(std::string(v));
        return j;
    }

    SystemUtils::DiagnosticsSender::UnsubscribeDelegate PgResult::SubscribeToDiagnostics(
        SystemUtils::DiagnosticsSender::DiagnosticMessageDelegate delegate, size_t minLevel) {
        return impl_->diagnosticsSender.SubscribeToDiagnostics(delegate, minLevel);
    }
}  // namespace Postgresql