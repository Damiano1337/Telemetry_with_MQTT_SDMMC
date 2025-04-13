#ifndef ZAPIS_NA_KARTE_H
#define ZAPIS_NA_KARTE_H

#include "pomiary.h"

void zapis_na_karte_init();
void pomiar_do_json(const Pomiary& pomiary);
void zapis_na_karte_flush();

#endif
