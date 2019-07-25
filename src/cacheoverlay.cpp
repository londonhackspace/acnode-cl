#include "cacheoverlay.h"

#include "macros.h"

CacheOverlay::CacheOverlay(Cache* base, Cache* overlay) :
    base(base),
    overlay(overlay)
{

}

CacheOverlay::~CacheOverlay() {

}

void CacheOverlay::begin() {
    base->begin();
    if(overlay) {
        overlay->begin();
    }
}

Card CacheOverlay::get(Card u) {
    if(overlay) {
        Card res = overlay->get(u);
        if(res.is_valid()) {
            DEBUG("HIT on overlay")
            return res;
        }
    }

    DEBUG("Using base cache")
    return base->get(u);
}

void CacheOverlay::set(const Card u) {
    base->set(u);
}

void CacheOverlay::purge(void) {
    base->purge();
}

int CacheOverlay::each(void( *callback)(Card u)) {
    int count = 0;
    if(overlay) {
        count = overlay->each(callback);
    }

    count += base->each(callback);
    return count;
}

void CacheOverlay::verify(void) {
    // only verify the base because we are treating the overlay as read-only
    base->verify();
}

void CacheOverlay::fill(void) {
    base->fill();
}

void CacheOverlay::fill(int count) {
    base->fill(count);
}

void CacheOverlay::list(void) {
    if(overlay) {
        overlay->list();
    }

    base->list();
}
