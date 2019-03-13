//
// Created by tate on 12-03-19.
//

#ifndef YS2_THREADS_HPP
#define YS2_THREADS_HPP

#include "../operators.hpp"

OP_NS(op_thread);
OP_NS(op_join);
OP_NS(op_sleep);
OP_NS(op_mutex);

OP_NS(op_quit); // moves feed to EOF, causing interpreter to end


/* if i could emulate promises in some way tht would be cool
 *
 * the async, await, .then, .catch system is powerful, but can be confusing to new users
 *
 * should still be able to work in non-async setting
 *
 *
 */

#endif //YS2_THREADS_HPP
