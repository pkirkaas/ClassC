#include "Set.h"

void pSet::republic(pSet * inset)
  {ff();
   inset->rewind();
   while (inset->next()) add((inset->now)->elemV);
  }
void cSet::republic(cSet * inset)
  {ff();
   inset->rewind();
   while (inset->next()) add((inset->now)->elemV);
  }
#endif
