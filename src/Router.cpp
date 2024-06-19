#include "Router.h"
#include "RoutePackage.h"


void Router::put(int pe_row_idx, int pe_col_idx, std::shared_ptr<RoutePackage> route_package){
    route_packages.push_back(route_package);
};

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