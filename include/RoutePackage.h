#ifndef DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
#define DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
class RoutePackage {
public:
    RoutePackage(int hops) {
        remaining_hops = hops;
    }
    virtual void operate() = 0;

private:
    int remaining_hops;
};

class CopyDataPackage: public RoutePackage {

};

class LoadSignalPackage: public RoutePackage {

};

class LoadDataPackage: public RoutePackage {

};

class StoreDataPackage: public RoutePackage {

};

class StoreAckPackage: public RoutePackage {

};
#endif //DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
