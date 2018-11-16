#include "acnode.h"

ACNode::ACNode(PN532 &n, RGB &r, Tool &t, int button_pin) :
  Role(n),
  rgb(r),
  tool(t),
  menu(r, button_pin),
  last_status_checked_at(-30000),
  enabled(false)
{
}

bool ACNode::card_present() {
  card_on_reader = readcard();
  return card_on_reader.is_valid();
}

bool ACNode::card_has_access() {
  Card cached = cache->get(card_on_reader);

  if (cached.compare_uid(card_on_reader)) {
    if (acsettings.netverbose == 1) {
      Serial.println("Card has been seen before (cached)");
    }
    // Use the cached version (TODO: restructure this)
    card_on_reader = cached;
    return cached.is_user() || cached.is_maintainer();
  }

  int status = networking::querycard(card_on_reader);
  switch (status) {
    case 2: // maintainer
      card_on_reader.set_maintainer(true);
    case 1: // user
      card_on_reader.set_user(true);
      Serial.println("Card has access");
      // Cache the card for future lookups
      cache->set(card_on_reader);
      return true;
    case 0:
      Serial.println("Card known, but has no access");
      return false;
    case -1:
      Serial.println("Card unknown");
      return false;
    default:
      Serial.println("A networking error has occurred.");
      return false;
  }
}

void ACNode::activate() {
  if (card_on_reader.is_maintainer()) {
    rgb.solid(YELLOW);
    tool.on(card_on_reader);
  } else {
    if (is_enabled()) {
      rgb.solid(GREEN);
      tool.on(card_on_reader);
    } else {
      rgb.solid(RED);
    }
  }
}

void ACNode::deactivate() {
  // TODO: Try and move this into role.h
  // Force a status check, TODO: add an optional param to is_enabled()
  last_status_checked_at = -30000;
  is_enabled() ? rgb.solid(BLUE) : rgb.solid(RED);
  tool.off();
}

void ACNode::housekeeping() {
  wdog.feed();
  rgb.run();
  tool.poll();
}

void ACNode::run() {
  housekeeping();
  if (menu.active()) {
    if (card_present()) {
      card_has_access(); // Read card permissions
      menu.run(&card_on_reader);
    } else {
      menu.reset();
    }
  } else {
    if (card_present()) {
      if (acsettings.netverbose == 1) {
        Serial.print("Card on reader: ");
        card_on_reader.dump();
      }

      // This is the entry point to the menu.
      if (card_has_access()) {
        menu.run(&card_on_reader);
        activate();
      } else {
        rgb.solid(RED);
      }
    } else {
      deactivate();
    }
  }
}

bool ACNode::is_enabled() {
  // Check status once every 30 seconds.
  if (millis() - last_status_checked_at > 30000) {
    last_status_checked_at = millis();
    int status = networking::networkCheckToolStatus();
    enabled = (status == 1);
    acsettings.status = status;
  }
  return enabled;
}

Card ACNode::readcard()
{
  //  pnhsu.intr_check();

  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  nfc.powerDown();

  if (!success)
  {
    return Card();
  }

  switch (uidLength) {
    case 4:
      return Card(uid, 0, 0, 0);
      break;
    case 7:
      return Card(uid, 1, 0, 0);
      break;
    default:
      Serial.print("Odd card length?: ");
      Serial.println(uidLength);
      break;
  }
  return Card();
}
