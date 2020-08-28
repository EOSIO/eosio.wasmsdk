#include <eosio/eosio.hpp>
#include <eosio/tester.hpp>

using namespace eosio;
using namespace eosio::native;

struct [[eosio::table]] person {
   uint64_t key;
   name user;
   uint64_t secondary1;
   uint64_t primary_key() const { return key; }
   uint64_t by_secondary_1() const { return secondary1; }
};
typedef eosio::multi_index<"people"_n, person,
   indexed_by<name("secondary1"), const_mem_fun<person, uint64_t, &person::by_secondary_1>>
> address_index;

EOSIO_TEST_BEGIN(multi_index_secondary_index_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = eosio::name{"bob"};
      row.secondary1 = 10;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = eosio::name{"alice"};
      row.secondary1 = 9;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = eosio::name{"john"};
      row.secondary1 = 8;
   });

   auto sec = addresses.get_index<name("secondary1")>();
   auto sitr = sec.find(8);
   CHECK_EQUAL(sitr->user, eosio::name{"john"});
   sitr++;
   CHECK_EQUAL(sitr->user, eosio::name{"alice"});
   sitr++;
   CHECK_EQUAL(sitr->user, eosio::name{"bob"});
   sitr--;
   CHECK_EQUAL(sitr->user, eosio::name{"alice"});

   auto itr = addresses.find(1);
   addresses.modify(itr, self, [&]( auto& row ) {
      row.user = name{"bruce"};
      row.secondary1 = 20;
   });

   sitr = sec.find(8);
   CHECK_EQUAL(sitr->user, eosio::name{"john"});
   sitr++;
   CHECK_EQUAL(sitr->user, eosio::name{"bruce"});

   sitr = sec.find(8);
   // TODO: Should I be able to use == here?
   CHECK_EQUAL(sitr->user, sec.begin()->user);

   sitr = sec.find(10);
   CHECK_NOT_EQUAL(sitr, sec.end());
   sec.erase(sitr);
   sitr = sec.find(10);
   CHECK_EQUAL(sitr, sec.end());
EOSIO_TEST_END

EOSIO_TEST_BEGIN(multi_index_secondary_remove_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = name{"a"};
      row.secondary1 = 10;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = name{"b"};
      row.secondary1 = 9;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = name{"c"};
      row.secondary1 = 8;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 3;
      row.user = name{"d"};
      row.secondary1 = 7;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 4;
      row.user = name{"e"};
      row.secondary1 = 6;
   });

   auto sec = addresses.get_index<name("secondary1")>();
   auto sitr = sec.find(6);
   sec.erase(sitr);

   sitr = sec.find(9);
   sec.erase(sitr);

   sitr = sec.find(6);
   CHECK_EQUAL(sitr, sec.end());

   sitr = sec.find(9);
   CHECK_EQUAL(sitr, sec.end());

   sitr = sec.find(8);
   CHECK_EQUAL(sitr->user, name{"c"});
   sitr++;
   CHECK_EQUAL(sitr->user, name{"a"});
EOSIO_TEST_END

EOSIO_TEST_BEGIN(multi_index_secondary_iterate_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = name{"a"};
      row.secondary1 = 10;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = name{"b"};
      row.secondary1 = 9;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = name{"c"};
      row.secondary1 = 8;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 3;
      row.user = name{"d"};
      row.secondary1 = 7;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 4;
      row.user = name{"e"};
      row.secondary1 = 6;
   });

   auto sec = addresses.get_index<name("secondary1")>();
   auto sitr = sec.find(6);
   CHECK_EQUAL(sitr->user, name{"e"});
   sitr++;
   CHECK_EQUAL(sitr->user, name{"d"});
   sitr++;
   CHECK_EQUAL(sitr->user, name{"c"});
   sitr++;
   CHECK_EQUAL(sitr->user, name{"b"});
   sitr++;
   CHECK_EQUAL(sitr->user, name{"a"});
   sitr++;
   CHECK_EQUAL(sitr, sec.end());
   REQUIRE_ASSERT("cannot increment end iterator", [&](){sitr++;});

   sitr = sec.find(10);
   CHECK_EQUAL(sitr->user, name{"a"});
   sitr--;
   CHECK_EQUAL(sitr->user, name{"b"});
   sitr--;
   CHECK_EQUAL(sitr->user, name{"c"});
   sitr--;
   CHECK_EQUAL(sitr->user, name{"d"});
   sitr--;
   CHECK_EQUAL(sitr->user, name{"e"});
   // TODO: Should I be able to use == here?
   CHECK_EQUAL(sitr->user, sec.begin()->user);
   REQUIRE_ASSERT("cannot decrement iterator at beginning of index" , [&](){sitr--;});
EOSIO_TEST_END

EOSIO_TEST_BEGIN(multi_index_secondary_bounds_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = name{"a"};
      row.secondary1 = 10;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = name{"b"};
      row.secondary1 = 9;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = name{"c"};
      row.secondary1 = 8;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 3;
      row.user = name{"d"};
      row.secondary1 = 7;
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 4;
      row.user = name{"e"};
      row.secondary1 = 6;
   });

   auto sec = addresses.get_index<name("secondary1")>();
   auto sitr = sec.lower_bound(9);
   CHECK_EQUAL(sitr->user, name{"b"});
   sitr--;
   CHECK_EQUAL(sitr->user, name{"c"});

   sitr = sec.upper_bound(9);
   CHECK_EQUAL(sitr->user, name{"e"});
   sitr++;
   CHECK_EQUAL(sitr->user, name{"d"});
EOSIO_TEST_END

int main(int argc, char** argv) {
   silence_output(false);
   // EOSIO_TEST(multi_index_secondary_index_test);
   // EOSIO_TEST(multi_index_secondary_remove_test);
   // EOSIO_TEST(multi_index_secondary_iterate_test);
   // EOSIO_TEST(multi_index_secondary_bounds_test);
   return has_failed();
}
