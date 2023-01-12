/*
 * The implementation of the GTPC graph data generator was built on
 * Florian Wolf's implementation of the CH-benCHmark data generator
 * (https://db.in.tum.de/research/projects/CHbenCHmark/) and 
 * Alexander van Renen's implementation of the TPC-C data generator
 * (https://github.com/alexandervanrenen/tpcc-generator)
 * See the README file.
 */

#ifndef generator_hpp_
#define generator_hpp_

#include <cstdint>
#include <string>
#include <random>

class GtpcGenerator {
   const static uint32_t kItemCount = 100000;
   const static uint32_t kCustomerPerDistrict = 3000;
   const static uint32_t kDistrictsPerWarehouse = 10;
   const static uint32_t OrdersPerDistrict = 3000;
   const static uint32_t RegionCount = 5;
   const static uint32_t NationCount = 62;
   const static uint32_t SupplierCount = 10000;

   // If these are different the order generation needs to be changed.
   // Right now there is a 1:1 relationship between customers and orders.
   static_assert(kCustomerPerDistrict == OrdersPerDistrict, "These should match, see comment.");

   const int64_t warehouse_count;
   const std::string folder;
   const std::string post_fix;

   std::mt19937 ranny;

   uint32_t setRegionName(int64_t id, int32_t max, char *dest);
   uint32_t makeAlphaString(uint32_t min, uint32_t max, char *dest);
   uint32_t makeNumberString(uint32_t min, uint32_t max, char *dest);
   uint32_t makeNumber(uint32_t min, uint32_t max);
   uint32_t makeNonUniformRandom(uint32_t A, uint32_t x, uint32_t y);
   std::vector<uint32_t> makePermutation(uint32_t min, uint32_t max);
   void makeAddress(char *str1, char *street2, char *city, char *state, char *zip);
   void makeLastName(int64_t num, char *name);
   void makeDate(uint32_t min, uint32_t max, char *str);
   void makeNow(char *str);

public:
   GtpcGenerator(int64_t warehouse_count, const std::string &folder);

   void setRandomSeed(uint32_t seed) { ranny.seed(seed); }

   void generateGraph();
   void generateWarehouses();
   void generateDistricts();
   void generateRegions();
   void generateNations();
   void generateCustomerAndHistory();
   void generateItems();
   void generateStock();
   void generateOrdersAndOrderLines();
   void generateSuppliers();
};

#endif