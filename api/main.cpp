#define CROW_STATIC_DIRECTORY "client/build/static/"
#include "crow.h"
#include "crow/middlewares/cors.h"
#include <algorithm>
#include <optional>

struct User
{
    int id;
    std::string username;
};
struct Database
{
    virtual std::vector<User> get_users() = 0;
    virtual std::optional<User> get_user_by_id(unsigned id) = 0;
};
struct TestDatabase : public Database
{
    std::vector<User> get_users() override
    {
        return {User{1, "cmastudios"}};
    }
    std::optional<User> get_user_by_id(unsigned id) override
    {
        if (id == 1) return User{1, "cmastudios"};
        return {};
    }
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
    CROW_BP_ROUTE(bp, "/<uint>")([&](unsigned id) -> crow::response {
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
    TestDatabase db;

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
