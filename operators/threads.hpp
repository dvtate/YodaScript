//
// Created by tate on 12-03-19.
//

#ifndef YS2_THREADS_HPP
#define YS2_THREADS_HPP

#include "../operator_ppcs.hpp"

OP_NS(op_thread);
OP_NS(op_join);
OP_NS(op_sleep);
OP_NS(op_mutex);
OP_NS(op_spawn);
OP_NS(op_quit); // moves feed to EOF, causing interpreter to end


/* if i could emulate js promises in some way tht would be cool
 *
 * the async, await, .then, .catch system is powerful, but can be confusing to new users
 *
 * should still be able to work in non-async setting
 *
 *
 */

/* How I'm thinking threads will work
 * Thread {
 * 		:create		# create a thread object
 * 		:spawn		# start thread
 * 		:sleep		# kill time (only applies to current thread)
 * 		:mutex		# somehow make a mutex system?
 * 		:kill		# kill a thread
 * 		:die		# end current thread
 * 		:join		# ?
 * 		:detach		# make it run independently
 * 		:cores		# $(nproc)
 * } namespace =
 *
 * $thread {
 * 		.id			# identifier
 * 		.alive		# is it currently running?
 * } object =
 *
 *
 */
#endif //YS2_THREADS_HPP
