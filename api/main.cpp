#include <stdexcept>
#define CROW_STATIC_DIRECTORY "client/build/static/"
#include "crow.h"
#include "crow/middlewares/cors.h"
#include <algorithm>
#include <optional>
#include <sqlite3.h>

struct User
{
    int id;
    std::string username;
};
struct Database
{
    virtual std::vector<User> get_users() = 0;
    virtual std::optional<User> get_user_by_id(int id) = 0;
};
struct TestDatabase : public Database
{
    std::vector<User> get_users() override
    {
        return {User{1, "cmastudios"}};
    }
    std::optional<User> get_user_by_id(int id) override
    {
        if (id == 1) return User{1, "cmastudios"};
        return {};
    }
};
struct Sqlite3StmtWrapper
{
    sqlite3_stmt *stmt { nullptr };
    ~Sqlite3StmtWrapper() { sqlite3_finalize(stmt); }
};
struct LocalDatabase : public Database
{
    LocalDatabase(std::string file)
    {
        if (sqlite3_open(file.c_str(), &db))
        {
            throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(db)));
        }
        char *execErr;
        if (sqlite3_exec(db, INIT_STMT, nullptr, nullptr, &execErr) != SQLITE_OK)
        {
            throw std::runtime_error("SQL error: " + std::string(execErr));
        }
    }
    virtual ~LocalDatabase()
    {
        sqlite3_close(db);
    }
    std::vector<User> get_users() override
    {
        Sqlite3StmtWrapper stmt;
        if (sqlite3_prepare_v2(db, "SELECT id, username FROM users", -1, &stmt.stmt, nullptr) != SQLITE_OK)
        {
            throw std::runtime_error("Prepare failure: " + std::string(sqlite3_errmsg(db)));
        }
        std::vector<User> users;
        int result;
        while ((result = sqlite3_step(stmt.stmt)) == SQLITE_ROW)
        {
            User user;
            user.id = sqlite3_column_int(stmt.stmt, 0);
            user.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt.stmt, 1));
            users.push_back(user);
        }
        if (result != SQLITE_DONE)
        {
            throw std::runtime_error("Step failure: " + std::string(sqlite3_errmsg(db)));
        }
        return users;
    }
    std::optional<User> get_user_by_id(int id) override
    {
        if (id == 1) return User{1, "cmastudios"};
        return {};
    }
private:
    sqlite3 *db { nullptr };
    const char *INIT_STMT =
        "CREATE TABLE IF NOT EXISTS users(id INTEGER PRIMARY KEY ASC, username TEXT);"
        "PRAGMA user_version = 1;"
    ;
};
crow::Blueprint user_blueprint(Database &db)
{
    crow::Blueprint bp("user");
    auto serialize_user = [](const User &user) -> crow::json::wvalue {
        return {{"id", user.id}, {"username", user.username}};
    };
    CROW_BP_ROUTE(bp, "/")([&]() -> crow::json::wvalue {
        auto users = db.get_users();
        std::vector<crow::json::wvalue> objects;
        std::transform(users.cbegin(), users.cend(), std::back_inserter(objects), serialize_user);
        return objects;
    });
    CROW_BP_ROUTE(bp, "/<int>")([&](int id) -> crow::response {
        auto user = db.get_user_by_id(id);
        if (user.has_value()) return crow::response(serialize_user(user.value()));
        return crow::response(404, crow::json::wvalue::empty_object());
    });
    return bp;
}

#define RETURN_STATIC_FILE(path) [](){ auto resp = crow::response(); resp.set_static_file_info(path); return resp; }

int main()
{
    crow::App<crow::CORSHandler> app;
    LocalDatabase db("app.db");

    CROW_ROUTE(app, "/")(RETURN_STATIC_FILE("client/build/index.html"));
    CROW_ROUTE(app, "/favicon.ico")(RETURN_STATIC_FILE("client/build/favicon.ico"));
    CROW_ROUTE(app, "/logo192.png")(RETURN_STATIC_FILE("client/build/logo192.png"));
    CROW_ROUTE(app, "/logo512.png")(RETURN_STATIC_FILE("client/build/logo512.png"));
    CROW_ROUTE(app, "/manifest.json")(RETURN_STATIC_FILE("client/build/manifest.json"));
    CROW_ROUTE(app, "/asset-manifest.json")(RETURN_STATIC_FILE("client/build/asset-manifest.json"));
    CROW_ROUTE(app, "/robots.txt")(RETURN_STATIC_FILE("client/build/robots.txt"));
    crow::Blueprint apibp("api");
    app.register_blueprint(apibp);
    CROW_BP_ROUTE(apibp, "/")([](){
        return 404;
    });
    auto userbp = user_blueprint(db);
    apibp.register_blueprint(userbp);

    app.port(4000).multithreaded().run();
}
