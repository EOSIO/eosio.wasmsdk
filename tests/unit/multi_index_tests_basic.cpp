#include <eosio/eosio.hpp>
#include <eosio/tester.hpp>

using namespace eosio;
using namespace eosio::native;

struct [[eosio::table]] person {
   uint64_t key;
   name user;
   uint64_t primary_key() const { return key; }
};
typedef eosio::multi_index<"people"_n, person> address_index;

EOSIO_TEST_BEGIN(multi_index_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = name{"bob"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = name{"alice"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = name{"john"};
   });

   auto itr = addresses.find(0);

   CHECK_EQUAL(itr->key, 0);
   CHECK_EQUAL(itr->user, name{"bob"});

   addresses.modify(itr, self, [&]( auto& row ) {
      row.user = name{"bruce"};
   });

   itr = addresses.find(0);

   CHECK_EQUAL(itr->key, 0);
   CHECK_EQUAL(itr->user, name{"bruce"});

   itr = addresses.find(0);
   CHECK_NOT_EQUAL(itr, addresses.end());

   addresses.erase(itr);
   itr = addresses.find(0);
   CHECK_EQUAL(itr, addresses.end());

   itr = addresses.find(1);
   itr++;
   CHECK_EQUAL(itr->user, name{"john"});
   itr--;
   CHECK_EQUAL(itr->user, name{"alice"});
EOSIO_TEST_END

EOSIO_TEST_BEGIN(multi_index_remove_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = name{"a"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = name{"b"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = name{"c"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 3;
      row.user = name{"d"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 4;
      row.user = name{"e"};
   });

   auto itr = addresses.find(0);
   addresses.erase(itr);

   itr = addresses.find(3);
   addresses.erase(itr);

   itr = addresses.find(0);
   CHECK_EQUAL(itr, addresses.end());

   itr = addresses.find(3);
   CHECK_EQUAL(itr, addresses.end());

   itr = addresses.find(2);
   CHECK_EQUAL(itr->user, name{"c"});
   itr++;
   CHECK_EQUAL(itr->user, name{"e"});

EOSIO_TEST_END

EOSIO_TEST_BEGIN(multi_index_iterate_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = name{"a"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = name{"b"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = name{"c"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 3;
      row.user = name{"d"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 4;
      row.user = name{"e"};
   });

   auto itr = addresses.find(0);
   CHECK_EQUAL(itr->user, name{"a"});
   itr++;
   CHECK_EQUAL(itr->user, name{"b"});
   itr++;
   CHECK_EQUAL(itr->user, name{"c"});
   itr++;
   CHECK_EQUAL(itr->user, name{"d"});
   itr++;
   CHECK_EQUAL(itr->user, name{"e"});
   itr++;
   CHECK_EQUAL(itr, addresses.end());
   REQUIRE_ASSERT("cannot increment end iterator", [&](){itr++;});

   itr = addresses.find(4);
   CHECK_EQUAL(itr->user, name{"e"});
   itr--;
   CHECK_EQUAL(itr->user, name{"d"});
   itr--;
   CHECK_EQUAL(itr->user, name{"c"});
   itr--;
   CHECK_EQUAL(itr->user, name{"b"});
   itr--;
   CHECK_EQUAL(itr->user, name{"a"});
   // TODO: Should I be able to use == here?
   CHECK_EQUAL(itr->user, addresses.begin()->user);
   REQUIRE_ASSERT("cannot decrement iterator at beginning of table" , [&](){itr--;});
EOSIO_TEST_END

EOSIO_TEST_BEGIN(multi_index_bounds_test)
   eosio::name n{"test"};
   contract_state cs = {n};
   apply_state(&cs);
   eosio::name self{"test"};
   intrinsics::set_intrinsic<intrinsics::current_receiver>([&]() { return self.value; });
   address_index addresses( self, self.value );

   addresses.emplace(self, [&]( auto& row ) {
      row.key = 0;
      row.user = name{"a"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 1;
      row.user = name{"b"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 2;
      row.user = name{"c"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 3;
      row.user = name{"d"};
   });
   addresses.emplace(self, [&]( auto& row ) {
      row.key = 4;
      row.user = name{"e"};
   });

   auto itr = addresses.lower_bound(3);
   CHECK_EQUAL(itr->user, name{"d"});
   itr--;
   CHECK_EQUAL(itr->user, name{"c"});

   itr = addresses.upper_bound(2);
   CHECK_EQUAL(itr->user, name{"a"});
   itr++;
   CHECK_EQUAL(itr->user, name{"b"});
EOSIO_TEST_END

int main(int argc, char** argv) {
   silence_output(false);
   // EOSIO_TEST(multi_index_test);
   // EOSIO_TEST(multi_index_remove_test);
   // EOSIO_TEST(multi_index_iterate_test);
   // EOSIO_TEST(multi_index_bounds_test);
   return has_failed();
}
