// rest_api/controllers/ThresholdsController.cc
#include "rest_api/controllers/ThresholdsController.h"
#include <drogon/HttpResponse.h>
#include <json/json.h>

namespace joshua::rest {

static Json::Value rule_to_json(const ThresholdRule& rule) {
    Json::Value v;
    v["id"]         = rule.id;
    v["module_id"]  = rule.module_id;
    v["test_id"]    = rule.test_id;
    v["metric_key"] = rule.metric_key;
    v["op"]         = rule.op;
    v["value"]      = rule.value_json;   // { "type": "T", "value": V }
    return v;
}

ThresholdsController::ThresholdsController(ThresholdService& svc) : svc_(svc) {}

void ThresholdsController::list(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
    std::string module_id = req->getParameter("module_id");
    if (module_id.empty()) {
        Json::Value err; err["error"] = "module_id query param required";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        cb(resp);
        return;
    }
    std::string test_id = req->getParameter("test_id");
    auto rules = svc_.list_rules(module_id, test_id);

    Json::Value arr{Json::arrayValue};
    for (const auto& r : rules) arr.append(rule_to_json(r));
    Json::Value resp;
    resp["rules"] = arr;
    resp["total"] = static_cast<int>(rules.size());
    cb(drogon::HttpResponse::newHttpJsonResponse(resp));
}

void ThresholdsController::create(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb) {
    auto body = req->getJsonObject();
    if (!body) {
        Json::Value err; err["error"] = "invalid json body";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        cb(resp);
        return;
    }
    for (const char* f : {"module_id", "metric_key", "op"}) {
        if (!(*body).isMember(f) || (*body)[f].asString().empty()) {
            Json::Value err; err["error"] = std::string("missing field: ") + f;
            auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(drogon::k422UnprocessableEntity);
            cb(resp);
            return;
        }
    }
    if (!(*body).isMember("value") || !(*body)["value"].isObject()) {
        Json::Value err; err["error"] = "missing field: value (typed {type, value} object)";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k422UnprocessableEntity);
        cb(resp);
        return;
    }

    std::string test_id = (*body).get("test_id", "").asString();
    try {
        auto rule = svc_.create_rule(
            (*body)["module_id"].asString(),
            test_id,
            (*body)["metric_key"].asString(),
            (*body)["op"].asString(),
            (*body)["value"]);
        auto resp = drogon::HttpResponse::newHttpJsonResponse(rule_to_json(rule));
        resp->setStatusCode(drogon::k201Created);
        cb(resp);
    } catch (const std::exception& e) {
        Json::Value err; err["error"] = e.what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k422UnprocessableEntity);
        cb(resp);
    }
}

void ThresholdsController::remove(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb,
    std::string id) {
    svc_.delete_rule(id);
    Json::Value resp; resp["ok"] = true;
    cb(drogon::HttpResponse::newHttpJsonResponse(resp));
}

}  // namespace joshua::rest
