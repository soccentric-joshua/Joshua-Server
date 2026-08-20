#pragma once
// rest_api/controllers/ThresholdsController.h — CRUD for metric threshold rules.
//
// Backed by ThresholdService (thresholds table, migration 0010).  Threshold rules
// are per (module_id, test_id, metric_key) with a typed op + value.  Crossing
// evaluation happens during result ingestion (RunService::ingest_test_result →
// ThresholdService::evaluate) and emits "result.threshold_crossed" notifications.

#include "services/ThresholdService.h"
#include <drogon/HttpController.h>
#include <functional>
#include <string>

namespace joshua::rest {

class ThresholdsController : public drogon::HttpController<ThresholdsController, false> {
public:
    explicit ThresholdsController(ThresholdService& svc);

    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ThresholdsController::list,   "/api/v1/thresholds",      drogon::Get);
    ADD_METHOD_TO(ThresholdsController::create, "/api/v1/thresholds",      drogon::Post);
    ADD_METHOD_TO(ThresholdsController::remove, "/api/v1/thresholds/{id}", drogon::Delete);
    METHOD_LIST_END

    // GET /api/v1/thresholds?module_id=X&test_id=Y
    void list(const drogon::HttpRequestPtr&,
              std::function<void(const drogon::HttpResponsePtr&)>&&);
    // POST /api/v1/thresholds  { module_id, test_id, metric_key, op, value:{type,value} }
    void create(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&);
    // DELETE /api/v1/thresholds/{id}
    void remove(const drogon::HttpRequestPtr&,
                std::function<void(const drogon::HttpResponsePtr&)>&&,
                std::string id);

private:
    ThresholdService& svc_;
};

}  // namespace joshua::rest
