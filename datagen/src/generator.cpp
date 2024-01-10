/*
 * The implementation of the GTPC graph data generator was built on
 * Florian Wolf's implementation of the CH-benCHmark data generator
 * (https://db.in.tum.de/research/projects/CHbenCHmark/) and 
 * Alexander van Renen's implementation of the TPC-C data generator
 * (https://github.com/alexandervanrenen/tpcc-generator)
 * See the README file.
 */

#include "generator.hpp"
#include "csv_writer.hpp"
#include "data_source.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cassert>
#include <cstring>

GtpcGenerator::GtpcGenerator(int64_t warehouse_count, const std::string &folder)
   : warehouse_count(warehouse_count), folder(folder), post_fix("_0_0.csv"), ranny(42) {
}

void GtpcGenerator::generateItems() {
   std::cout << "Generating 'Item' node objects .. " << std::flush;
   std::string header_i = "id|im_id|name|price|data";

   int64_t i_id;
   std::array<char, 24> i_name = {};
   float i_price;
   std::array<char, 50> i_data = {};
   uint32_t i_data_size;
   std::vector<bool> orig(kItemCount, false);
   int64_t i_im_id;

   csv::CsvWriter i_csv(folder + "/item" + post_fix);
   i_csv << header_i << csv::endl;

   for (uint32_t i = 0; i<kItemCount / 10; i++) {
      uint32_t pos;
      do {
         pos = makeNumber(0L, kItemCount - 1);
      } while (orig[pos]);
      orig[pos] = true;
   }

   for (i_id = 1; i_id<=kItemCount; i_id++) {
      makeAlphaString(14, 24, i_name.data());
      i_price = ((float) makeNumber(100L, 10000L)) / 100.0f;
      i_data_size = makeAlphaString(26, 50, i_data.data());
      i_im_id = makeNumber(0, 10000);
      if (orig[i_id]) {
         uint32_t pos = makeNumber(0L, i_data_size - 8);
         i_data[pos] = 'o';
         i_data[pos + 1] = 'r';
         i_data[pos + 2] = 'i';
         i_data[pos + 3] = 'g';
         i_data[pos + 4] = 'i';
         i_data[pos + 5] = 'n';
         i_data[pos + 6] = 'a';
         i_data[pos + 7] = 'l';
      }

      // @formatter:off
      i_csv << i_id << i_im_id << i_name << csv::Precision(2) << i_price << i_data << csv::endl;
      // @formatter:on
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateWarehouses() {
   std::cout << "Generating 'Warehouse' node objects .. " << std::flush;
   std::string header_w = "id|name|street_1|street_2|city|state|zip|tax|ytd";

   int64_t w_id;
   std::array<char, 10> w_name = {};
   std::array<char, 20> w_street_1 = {};
   std::array<char, 20> w_street_2 = {};
   std::array<char, 20> w_city = {};
   std::array<char, 2> w_state = {};
   std::array<char, 9> w_zip = {};
   float w_tax;
   float w_ytd;

   csv::CsvWriter w_csv(folder + "/warehouse" + post_fix);
   w_csv << header_w << csv::endl;

   for (w_id = 1L; w_id<=warehouse_count; w_id++) {
      makeAlphaString(6, 10, w_name.data());
      makeAddress(w_street_1.data(), w_street_2.data(), w_city.data(), w_state.data(), w_zip.data());
      w_tax = ((float) makeNumber(10L, 20L)) / 100.0f;
      w_ytd = 3000000.00f;

      // @formatter:off
      w_csv << w_id << w_name << w_street_1 << w_street_2 << w_city << w_state << w_zip << csv::Precision(4)
            << w_tax << csv::Precision(2) << w_ytd << csv::endl;
      // @formatter:on
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateDistricts() {
   std::cout << "Generating 'District' node objects and ':covers' relationship objects .. " << std::flush;
   std::string header_d = "id|name|street_1|street_2|city|state|zip|tax|ytd|next_o_id";
   std::string header_covers = "Warehouse_id|District_id";

   int64_t d_id;
   int64_t d_w_id;
   std::array<char, 10> d_name = {};
   std::array<char, 20> d_street_1 = {};
   std::array<char, 20> d_street_2 = {};
   std::array<char, 20> d_city = {};
   std::array<char, 2> d_state = {};
   std::array<char, 9> d_zip = {};
   float d_tax;
   float d_ytd;
   int64_t d_next_o_id;

   csv::CsvWriter d_csv(folder + "/district" + post_fix);
   csv::CsvWriter covers_csv(folder + "/warehouse_covers_district" + post_fix);
   d_csv << header_d << csv::endl;
   covers_csv << header_covers << csv::endl;

   // Each warehouse has DIST_PER_WARE (10) districts
   int64_t id = 0;
   for (d_w_id = 1; d_w_id<=warehouse_count; d_w_id++) {
      for (d_id = 1; d_id<=kDistrictsPerWarehouse; d_id++) {
         id++;
         d_ytd = 30000.0;
         d_next_o_id = 3001L;
         makeAlphaString(6L, 10L, d_name.data());
         makeAddress(d_street_1.data(), d_street_2.data(), d_city.data(), d_state.data(), d_zip.data());
         d_tax = ((float) makeNumber(10L, 20L)) / 100.0f;

         // @formatter:off
         d_csv << id /*<< d_w_id*/ << d_name << d_street_1 << d_street_2 << d_city << d_state << d_zip << csv::Precision(4)
               << d_tax << csv::Precision(2) << d_ytd << d_next_o_id << csv::endl;
         covers_csv << d_w_id << id << csv::endl;
         // @formatter:on
      }
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateCustomerAndHistory() {
   std::cout << "Generating 'Customer' node objects and ':serves', ':cIsLocatedIn' relationship objects .. " << std::flush;
   std::string header_c = "id|first|middle|last|street_1|street_2|city|state|zip|phone|since|"
                        "credit|credit_lim|discount|balance|ytd_payment|payment_cnt|delivery_cnt|data|"
                        "history_date|history_amount|history_data";
   std::string header_serves = "District_id|Customer_id";
   std::string header_isLocatedIn = "Customer_id|Nation_id";

   int64_t c_id;
   int64_t c_d_id;
   int64_t c_w_id;
   std::array<char, 16> c_first = {};
   std::array<char, 2> c_middle = {};
   std::array<char, 16> c_last = {};
   std::array<char, 20> c_street_1 = {};
   std::array<char, 20> c_street_2 = {};
   std::array<char, 20> c_city = {};
   std::array<char, 2> c_state = {};
   std::array<char, 9> c_zip = {};
   std::array<char, 16> c_phone = {};
   // std::array<char, 15> c_since = {}; // XXX used in history and customer and generated over and over again
   std::array<char, 28> c_since = {}; // XXX used in history and customer and generated over and over again
   std::array<char, 2> c_credit = {};
   float c_credit_lim;
   float c_discount;
   float c_balance;
   std::array<char, 500> c_data = {};
   std::array<char, 28> h_date = {};
   float h_amount;
   std::array<char, 24> h_data = {};

   csv::CsvWriter c_csv(folder + "/customer" + post_fix);
   // csv::CsvWriter h_csv(folder + "/history" + post_fix);
   csv::CsvWriter serves_csv(folder + "/district_serves_customer" + post_fix);
   csv::CsvWriter isLocatedIn_csv(folder + "/customer_isLocatedIn_nation" + post_fix);
   c_csv << header_c << csv::endl;
   serves_csv << header_serves << csv::endl;
   isLocatedIn_csv << header_isLocatedIn << csv::endl;

   // Each warehouse has DIST_PER_WARE (10) districts
   int64_t id1 = 0;
   int64_t id2 = 0;
   for (c_w_id = 1; c_w_id<=warehouse_count; c_w_id++) {
      for (c_d_id = 1; c_d_id<=kDistrictsPerWarehouse; c_d_id++) {
         id1++;
         for (c_id = 1; c_id<=kCustomerPerDistrict; c_id++) {
            id2++;
            makeAlphaString(8, 16, c_first.data());
            c_middle[0] = 'O';
            c_middle[1] = 'E';
            if (c_id<=1000)
               makeLastName(c_id - 1, c_last.data());
            else
               makeLastName(makeNonUniformRandom(255, 0, 999), c_last.data());
            makeAddress(c_street_1.data(), c_street_2.data(), c_city.data(), c_state.data(), c_zip.data());
            makeNumberString(16, 16, c_phone.data());
            c_credit[0] = makeNumber(0L, 1L) == 0 ? 'G' : 'B';
            c_credit[1] = 'C';
            c_credit_lim = 50000;
            c_discount = ((float) makeNumber(0L, 50L)) / 100.0f;
            c_balance = -10.0f;
            // makeNow(c_since.data());
            makeDate(1993, 2012, c_since.data());
            makeAlphaString(300, 500, c_data.data());

            makeDate(2012, 2012, h_date.data());
            h_amount = 10.0;
            makeAlphaString(12, 24, h_data.data());

            char n_char = *c_state.data();
            int64_t nid = (int64_t)n_char;

            // @formatter:off
            c_csv << id2 /*<< c_d_id << c_w_id*/ << c_first << c_middle << c_last << c_street_1 << c_street_2 << c_city
                  << c_state << c_zip << c_phone << c_since << c_credit << csv::Precision(2) << c_credit_lim
                  << csv::Precision(4) << c_discount << csv::Precision(2) << c_balance << 10.0f << int64_t(1)
                  << int64_t(0) << c_data << h_date << h_amount << h_data << csv::endl;
            // @formatter:on

            serves_csv << id1 << id2 << csv::endl;
            isLocatedIn_csv << id2 << nid << csv::endl;

            // // @formatter:off
            // h_csv << c_id << c_d_id << c_w_id << c_d_id << c_w_id << c_since << h_amount << h_data << csv::endl;
            // // @formatter:on
         }
      }
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateStock() {
   std::cout << "Generating 'Stock' node objects and ':wHasStock', ':iHasStock', ':hasSupplier' relationship objects .. " << std::flush;
   std::string header_s = "id|quantity|dist_01|dist_02|dist_03|dist_04|dist_05|dist_06|dist_07|dist_08|dist_09|"
                        "dist_10|ytd|order_cnt|remote_cnt|data";
   std::string header_wHasStock = "Warehouse_id|Stock_id";
   std::string header_iHasStock = "Item_id|Stock_id";
   std::string header_hasSupplier = "Stock_id|Supplier_id";

   int64_t s_i_id;
   int64_t s_w_id;
   int64_t s_quantity;
   std::array<char, 24> s_dist_01 = {};
   std::array<char, 24> s_dist_02 = {};
   std::array<char, 24> s_dist_03 = {};
   std::array<char, 24> s_dist_04 = {};
   std::array<char, 24> s_dist_05 = {};
   std::array<char, 24> s_dist_06 = {};
   std::array<char, 24> s_dist_07 = {};
   std::array<char, 24> s_dist_08 = {};
   std::array<char, 24> s_dist_09 = {};
   std::array<char, 24> s_dist_10 = {};
   int64_t s_ytd = 0;
   int64_t s_order_cnt = 0;
   int64_t s_remote_cnt = 0;
   std::array<char, 50> s_data = {};
   std::vector<bool> orig(kItemCount, false);

   csv::CsvWriter s_csv(folder + "/stock" + post_fix);
   csv::CsvWriter wHasStock_csv(folder + "/warehouse_hasStock_stock" + post_fix);
   csv::CsvWriter iHasStock_csv(folder + "/item_hasStock_stock" + post_fix);
   csv::CsvWriter hasSupplier_csv(folder + "/stock_hasSupplier_supplier" + post_fix);
   s_csv << header_s << csv::endl;
   wHasStock_csv << header_wHasStock << csv::endl;
   iHasStock_csv << header_iHasStock << csv::endl;
   hasSupplier_csv << header_hasSupplier << csv::endl;

   int64_t id = 0;
   for (s_w_id = 1; s_w_id<=warehouse_count; s_w_id++) {
      orig.assign(kItemCount, false);

      for (uint32_t i = 0; i<kItemCount / 10; i++) {
         int64_t pos;
         do {
            pos = makeNumber(0L, kItemCount - 1);
         } while (orig[pos]);
         orig[pos] = 1;
      }

      for (s_i_id = 1; s_i_id<=kItemCount; s_i_id++) {
         id++;
         s_quantity = makeNumber(10L, 100L);
         makeAlphaString(24, 24, s_dist_01.data());
         makeAlphaString(24, 24, s_dist_02.data());
         makeAlphaString(24, 24, s_dist_03.data());
         makeAlphaString(24, 24, s_dist_04.data());
         makeAlphaString(24, 24, s_dist_05.data());
         makeAlphaString(24, 24, s_dist_06.data());
         makeAlphaString(24, 24, s_dist_07.data());
         makeAlphaString(24, 24, s_dist_08.data());
         makeAlphaString(24, 24, s_dist_09.data());
         makeAlphaString(24, 24, s_dist_10.data());
         uint32_t s_data_size = makeAlphaString(26, 50, s_data.data());
         int64_t s_su_id = (s_i_id*s_w_id)%(SupplierCount);
         if (orig[s_i_id]) {
            int64_t pos = makeNumber(0L, s_data_size - 8);
            s_data[pos] = 'o';
            s_data[pos + 1] = 'r';
            s_data[pos + 2] = 'i';
            s_data[pos + 3] = 'g';
            s_data[pos + 4] = 'i';
            s_data[pos + 5] = 'n';
            s_data[pos + 6] = 'a';
            s_data[pos + 7] = 'l';
         }

         // @formatter:off
         s_csv << id /*<< s_i_id << s_w_id*/ << s_quantity << s_dist_01 << s_dist_02 << s_dist_03 << s_dist_04 << s_dist_05
               << s_dist_06 << s_dist_07 << s_dist_08 << s_dist_09 << s_dist_10 << s_ytd << s_order_cnt
               << s_remote_cnt << s_data << csv::endl;
         // @formatter:on
         wHasStock_csv << s_w_id << id << csv::endl;
         iHasStock_csv << s_i_id << id << csv::endl;
         hasSupplier_csv << id << s_su_id << csv::endl;
      }
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateOrdersAndOrderLines() {
   std::cout << "Generating 'Order', 'OrderLine' node objects and ':hasPlaced', ':olHasStock', ':contains' relationship objects .. " << std::flush;
   std::string header_o = "id|entry_d|carrier_id|ol_cnt|all_local|new_order";
   std::string header_ol = "id|number|delivery_d|quantity|amount|dist_info";
   std::string header_hasPlaced = "Customer_id|Order_id";
   std::string header_olHasStock = "OrderLine_id|Stock_id";
//    std::string header_hasItem = "OrderLine.id|Item.id";
   std::string header_contains = "Order_id|OrderLine_id";

   int64_t o_id;
   int64_t o_c_id;
   int64_t o_d_id;
   int64_t o_w_id;
   int64_t o_carrier_id;
   int64_t o_ol_cnt;
   std::array<char, 28> o_entry_d = {}; // XXX not sure if date is generate correctly
   int64_t o_all_local = 1;

   int64_t ol_number;
   int64_t ol_i_id;
   int64_t ol_s_id;
   std::array<char, 28> ol_del_d = {}; // XXX not sure if date is generate correctly
   int64_t ol_quantity;
   float ol_amount;
   std::array<char, 24> ol_dist_info = {};
   std::string kNull = "0";
   std::string kNullDate = "1970-01-01T00:00:00.000+0000";

   csv::CsvWriter o_csv(folder + "/order" + post_fix);
   csv::CsvWriter ol_csv(folder + "/orderLine" + post_fix);
   // csv::CsvWriter no_csv(folder + "/newOrder" + post_fix);
   csv::CsvWriter hasPlaced_csv(folder + "/customer_hasPlaced_order" + post_fix);
   csv::CsvWriter olHasStock_csv(folder + "/orderLine_hasStock_stock" + post_fix);
//    csv::CsvWriter hasItem_csv(folder + "/orderLine_hasItem_item" + post_fix);
   csv::CsvWriter contains_csv(folder + "/order_contains_orderLine" + post_fix);
   o_csv << header_o << csv::endl;
   ol_csv << header_ol << csv::endl;
   hasPlaced_csv << header_hasPlaced << csv::endl;
   olHasStock_csv << header_olHasStock << csv::endl;
//    hasItem_csv << header_hasItem << csv::endl;
   contains_csv << header_contains << csv::endl;

    // Each customer has exactly one order
    uint32_t permutation_range = warehouse_count * kDistrictsPerWarehouse * kCustomerPerDistrict;
    std::vector<uint32_t> customer_id_permutation = makePermutation(1, permutation_range + 1);

   // Generate ORD_PER_DIST (3000) orders and order line items for each district
   int64_t id1 = 0;
   int64_t id2 = 0;
   int64_t id3 = 0;
   for (o_w_id = 1L; o_w_id<=warehouse_count; o_w_id++) {
      for (o_d_id = 1L; o_d_id<=kDistrictsPerWarehouse; o_d_id++) {

         int64_t id4 = 0;
         //  for (o_id = 1; o_id<=OrdersPerDistrict; o_id++) {
         for (o_c_id = 1; o_c_id<=kCustomerPerDistrict; o_c_id++) {
            id1++;
            id2 = customer_id_permutation[id1 - 1];
            o_carrier_id = makeNumber(1L, 10L);
            // o_ol_cnt = DataSource::nextOderlineCount();
            o_ol_cnt = makeNumber(5L, 15L);
            // makeNow(o_entry_d.data());
            makeDate(2010, 2012, o_entry_d.data());

            id4++;
            // @formatter:off
             o_csv << id1 /*<< o_d_id << o_w_id << o_c_id*/ << o_entry_d << (id4>2100 ? kNull : std::to_string(o_carrier_id))
                        << o_ol_cnt << o_all_local << (id4>2100 ? (int64_t)1 : (int64_t)0) << csv::endl;
            // @formatter:on

            hasPlaced_csv << id2 << id1 << csv::endl;

            // Order line items
            for (ol_number = 1; ol_number<=o_ol_cnt; ol_number++) {
               id3++;
               ol_i_id = makeNumber(1L, kItemCount);
               ol_s_id = (kItemCount*(o_w_id-1)) + ol_i_id;
               ol_quantity = 5;
               makeAlphaString(24, 24, ol_dist_info.data());
               makeDate(2011, 2012, ol_del_d.data());

               if (id4>2100) {
                  ol_amount = (float) (makeNumber(10L, 10000L)) / 100.0f;
                  // @formatter:off
                  ol_csv << id3 /*<< o_id << o_d_id << o_w_id*/ << ol_number /*<< ol_i_id << o_w_id*/ << kNullDate
                         << ol_quantity << csv::Precision(2) << ol_amount << ol_dist_info << csv::endl;
                  // @formatter:on
               } else {
                  ol_amount = 0.0f;
                  // @formatter:off
                  ol_csv << id3 /*<< o_id << o_d_id << o_w_id*/ << ol_number /*<< ol_i_id << o_w_id*/
                         << ol_del_d << ol_quantity << csv::Precision(2) << ol_amount << ol_dist_info << csv::endl;
                  // @formatter:on
               }
               contains_csv << id1 << id3 << csv::endl;
               olHasStock_csv << id3 << ol_s_id <<  csv::endl;
            }

            // Generate a new order entry for the order for the last 900 rows
            // if (o_id>2100) {
            //    no_csv << o_id << o_d_id << o_w_id << csv::endl;
            // }
         }
      }
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateRegions() {
   std::cout << "Generating 'Region' node objects .. " << std::flush;
   std::string header_r = "id|name|comment";

   int64_t r_id;
   std::array<char, 25> r_name = {};
   std::array<char, 152> r_comment = {};

   csv::CsvWriter r_csv(folder + "/region" + post_fix);
   r_csv << header_r << csv::endl;

   for (r_id = 0L; r_id<RegionCount; r_id++) {
      setRegionName(r_id, 25, r_name.data());
      makeAlphaString(80, 152, r_comment.data());

      // @formatter:off
      r_csv << r_id << r_name << r_comment << csv::endl;
      // @formatter:on
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateNations() {
   std::cout << "Generating 'Nation' node objects and ':isPartOf' relationship objects .. " << std::flush;
   std::string header_n = "id|name|comment";
   std::string header_isPartOf = "Nation_id|Region_id";

   int64_t n_id;
   std::array<char, 25> n_name = {};
   std::array<char, 152> n_comment = {};
   const static char *nation_keys = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

   csv::CsvWriter n_csv(folder + "/nation" + post_fix);
   csv::CsvWriter isPartOf_csv(folder + "/nation_isPartOf_region" + post_fix);
   n_csv << header_n << csv::endl;
   isPartOf_csv << header_isPartOf << csv::endl;

   for (n_id = 0L; n_id<NationCount; n_id++) {
      // makeAlphaString(13, 25, n_name.data());
      Nation n = DataSource::getNation(n_id);
      makeAlphaString(80, 152, n_comment.data());
      // char n_key = nation_keys[n_id];
      // int64_t id = (int64_t)n_key;

      // @formatter:off
      // n_csv << id << n_name << n_comment << csv::endl;
      n_csv << (int64_t)n.id << n.name << n_comment << csv::endl;
      // @formatter:on
      isPartOf_csv << (int64_t)n.id << (int64_t)n.rId << /*id%(RegionCount+1) <<*/ csv::endl;
   }

   std::cout << "done." << std::endl;
}

void GtpcGenerator::generateSuppliers() {
   std::cout << "Generating 'Supplier' node objects and ':sIsLocatedIn' relationship objects .. " << std::flush;
   std::string header_su = "id|name|address|phone|acctbal|comment";
   std::string header_isLocatedIn = "Supplier_id|Nation_id";

   int64_t su_id;
   std::array<char, 25> su_name = {};
   std::array<char, 40> su_addr = {};
   std::array<char, 16> su_phone = {};
   std::array<char, 101> su_comment = {};
   float su_acct_bal;

   const static char *n_keys = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

   csv::CsvWriter su_csv(folder + "/supplier" + post_fix);
   csv::CsvWriter isLocatedIn_csv(folder + "/supplier_isLocatedIn_nation" + post_fix);
   su_csv << header_su << csv::endl;
   isLocatedIn_csv << header_isLocatedIn << csv::endl;

   for (su_id = 1; su_id<=SupplierCount; su_id++) {
      makeAlphaString(14, 24, su_name.data());
      makeAlphaString(20, 40, su_addr.data());
      makeAlphaString(50, 101, su_comment.data());
      makeNumberString(16, 16, su_phone.data());
      su_acct_bal = ((float) makeNumber(1000L, 10000L)) / 1.0f;

      // char nkey = n_keys[ranny() % 62];
      // int64_t nid = (int64_t)nkey;
      Nation n = DataSource::getNation((ranny() % 62));

      // @formatter:off
      su_csv << su_id << su_name << su_addr << su_phone << csv::Precision(2) << su_acct_bal << su_comment << csv::endl;
      // @formatter:on
      isLocatedIn_csv << su_id << (int64_t)n.id << /*nid <<*/ csv::endl;
   }

   std::cout << "done." << std::endl;
}

uint32_t GtpcGenerator::setRegionName(int64_t idx, int32_t max, char *dest) {
   const char *name = DataSource::getRegion(idx);

   uint32_t len = strlen(name);
   for (uint32_t i = 0; i<len; i++) {
      dest[i] = name[i];
   }
   if (len<(uint32_t)max) {
      dest[len] = '\0';
   }
   return len;
}

uint32_t GtpcGenerator::makeAlphaString(uint32_t min, uint32_t max, char *dest) {
   const static char *possible_values = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

   uint32_t len = makeNumber(min, max);
   for (uint32_t i = 0; i<len; i++) {
      dest[i] = possible_values[ranny() % 62];
   }
   if (len<max) {
      dest[len] = '\0';
   }
   return len;
}

uint32_t GtpcGenerator::makeNumberString(uint32_t min, uint32_t max, char *dest) {
   const static char *possible_values = "0123456789";

   uint32_t len = makeNumber(min, max);
   for (uint32_t i = 0; i<len; i++) {
      dest[i] = possible_values[ranny() % 10];
   }
   if (len<max) {
      dest[len] = '\0';
   }
   return len;
}

void GtpcGenerator::makeDate(uint32_t min, uint32_t max, char *str) {
   // TODOmakeNumber
   std::vector<std::string> mos = {"01", "02", "03", "04", "05", "06",
                                    "07", "08", "09", "10", "11", "12"};
   std::string yr = std::to_string(makeNumber(min, max));
   std::string mo = mos[ranny() % 11];
   std::string d = std::to_string(makeNumber(10, 28));
   // string s = "2010-02-14T15:32:10.447+0000";
   std::string dt = yr + "-" + mo + "-" + d + "T15:32:10.447+0000";
   strncpy(str, dt.data(), dt.size());
}

void GtpcGenerator::makeAddress(char *street1, char *street2, char *city, char *state, char *zip) {
   makeAlphaString(10, 20, street1);
   makeAlphaString(10, 20, street2);
   makeAlphaString(10, 20, city);
   makeAlphaString(2, 2, state);
   makeNumberString(9, 9, zip); // XXX
}

uint32_t GtpcGenerator::makeNumber(uint32_t min, uint32_t max) {
   return ranny() % (max - min + 1) + min;
}

uint32_t GtpcGenerator::makeNonUniformRandom(uint32_t A, uint32_t x, uint32_t y) {
   return ((makeNumber(0, A) | makeNumber(x, y)) + 42) % (y - x + 1) + x; // XXX
}

std::vector<uint32_t> GtpcGenerator::makePermutation(uint32_t min, uint32_t max) {
   assert(max>min);
   const uint32_t count = max - min;
   std::vector<uint32_t> result(count);
   iota(result.begin(), result.end(), min);

   for (uint32_t i = 0; i<count; i++) {
      std::swap(result[i], result[ranny() % count]);
   }
   return result;
}

void GtpcGenerator::makeLastName(int64_t num, char *name) {
   static const char *n[] = {"BAR", "OUGHT", "ABLE", "PRI", "PRES", "ESE", "ANTI", "CALLY", "ATION", "EING"};
   strcpy(name, n[num / 100]);
   strcat(name, n[(num / 10) % 10]);
   strcat(name, n[num % 10]);
}

void GtpcGenerator::makeNow(char *str) {
   std::string s = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()); // XXX
   strncpy(str, s.data(), s.size());
}
