#include <iostream>
#include <regex>
#include <random>
#include "CLI/CLI.hpp"

#include "generator.hpp"

#define GTPC_VERSION 0.9

// using namespace boost::program_options;

int main(int argc, char **argv) {
  std::string directory = ".";
  std::size_t warehouses;

  CLI::App app{"GTPC Graph Database Benchmark Generator"};

  app.add_option("-d,--directory", directory, "Path to out directory for generated GTPC CSV files")->required();
  app.add_option("-w,--warehouses", warehouses, "Number of warehouses")->required();

  CLI11_PARSE(app, argc, argv);

  std::string wstr = (warehouses > 1) ? "warehouses" : "warehouse";
  std::cout << "--------- Generating GTPC data with " << warehouses << " " << wstr << std::endl;
  auto start = std::chrono::steady_clock::now();

  GtpcGenerator generator((uint32_t)warehouses, directory);
  generator.generateWarehouses();
  generator.generateDistricts();
  generator.generateCustomerAndHistory();
  generator.generateItems();
  generator.generateSuppliers();
  generator.generateStock();
  generator.generateOrdersAndOrderLines();
  generator.generateRegions();
  generator.generateNations();

  auto end = std::chrono::steady_clock::now();
  double t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "--------- Data generation completed in " << t << " msecs." << std::endl;

  return 0;
}
