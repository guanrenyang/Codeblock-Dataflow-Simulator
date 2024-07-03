#include "Router.h"
#include "RoutePackage.h"


void Router::put(std::shared_ptr<RoutePackage> route_package){
    buffered_route_packages.push_back(route_package);
};

void Router::issue_buffered_route_packages() {
    route_packages.splice(route_packages.end(), buffered_route_packages);
}

void Router::execute_cycle() {
    // traverse the set
    for (auto it = route_packages.begin(); it != route_packages.end(); ) {
        (*it)->execute_cycle(shared_from_this());
        if ((*it)->is_completed()) {
            it = route_packages.erase(it);
        } else {
            ++it;
        }
    }
}