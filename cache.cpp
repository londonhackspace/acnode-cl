#include "cache.h"
#include "acnode.h"

// fill the card cache with junk users to see how well we cope with runing out of space.
void Cache::fill(void) {
  fill(1000);
}

void Cache::fill(const int count) {
  int i;
  uint8_t uid[7];

  for (i = 0 ; i < count ; i++) {
    uid[0] = i & 0xff;
    uid[1] = (i >> 8) & 0xff;
    Card u(uid, 0, 1, 0);

    set(u);
    wdog.feed();
  }
}

void list_callback(Card c) {
  c.dump();
}

void Cache::list(void) {
  int count = each(&list_callback);
  Serial.print("currently storing: ");
  Serial.print(count);
  Serial.println(" users");
}

