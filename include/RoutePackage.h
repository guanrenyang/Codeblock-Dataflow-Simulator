#ifndef DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
#define DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
class RoutePackage {
public:
    RoutePackage() {}
    virtual void operate() = 0;

private:
    int remaining_hops;
};

class CopyDataPackage: public RoutePackage {
public:
    CopyDataPackage(int src_pe_row, int src_pe_col, int dst_pe_row, int dst_pe_col) {
        remaining_hops =
    }
private:
    int dst_pe_row_idx;
    int dst_pe_col_idx;
    // Write to reg
};

class LoadSignalPackage: public RoutePackage {
    // Read SPM
};

class LoadDataPackage: public RoutePackage {
    // Write to reg
};

class StoreDataPackage: public RoutePackage {
    // Write to SPM
};

class StoreAckPackage: public RoutePackage {

};
#endif //DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
