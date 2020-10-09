/*
 * This file is part of Auriga HL7 library.
 *
 * Auriga HL7 library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Auriga HL7 library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Auriga HL7 library.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __QCK_Q02__24_H__
#define __QCK_Q02__24_H__

#include "../../common/Util.h"

namespace HL7_24 {

/* QCK_Q02 */
struct QCK_Q02 : public HL7Message {
  QCK_Q02() { this->init(); }
  /* Fields ID  */
  enum FIELD_ID { FIELD_ID_MAX };

  const char* className() const { return "QCK_Q02"; }
  QCK_Q02* create() const { return new QCK_Q02(); }

 private:
  /* Initialize object */
  void init() { setName("QCK_Q02"); }

 public:
  /* Getters list */

  /* Checker list */
}; /* QCK_Q02_ */
} /* namespace HL7_24 */
#endif /*  __QCK_Q02__24_H__ */
