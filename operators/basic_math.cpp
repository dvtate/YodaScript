
#include <cmath>
#include "basic_math.hpp"


namespace op_add {
	const char* name = "+";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "+ expected 2 values", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();

		const Value* v1 = f.stack.back().defer();
		if (!v1)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "null/cyclic reference passed to + operator", f.feed.lineNumber());

		auto TypeError = [&](Value::vtype t1, Value::vtype t2) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "invalid addition of "
				+ std::string(Value::typeName(t1)) + " & " + Value::typeName(t2), f.feed.lineNumber());
		};

		switch (v1->type) {

		case Value::OBJ: {

			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			f.stack.emplace_back(std::move(v2));
			if (!v1->obj->callMember(f, "__operator+", ev, self, 1))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__operator+", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
			} break;
		case Value::STR:
			switch (v2.type) {
			case Value::STR:
				f.stack.back().set(*v1->str + *v2.str);
				break;
			case Value::INT:
				f.stack.back().set(*v1->str + v2.mp_int->get_str(10));
				break;
			case Value::DEC:
				f.stack.back().set(*v1->str + std::to_string(v2.dec)); // maybe change this ...
				break;
			default: // oop operator overloading coming soon...
				return TypeError(v1->type, v2.type);
			}
			break;

		case Value::INT:
			switch (v2.type) {
			case Value::STR:
				f.stack.back().set(v1->mp_int->get_str(10) + *v2.str);
				break;
			case Value::DEC:
				f.stack.back().set(v1->mp_int->get_d() + v2.dec);
				break;
			case Value::INT:
				f.stack.back().set(mpz_class(*v1->mp_int + *v2.mp_int));
				break;
			default:
				return TypeError(v1->type, v2.type);
			}
			break;

		case Value::DEC:
			switch (v2.type) {
			case Value::STR:
				f.stack.back().set(std::to_string(v1->dec) + *v2.str);
				break;
			case Value::INT:
				f.stack.back().set(v1->dec + v2.mp_int->get_d());
				break;
			case Value::DEC:
				f.stack.back().set(v1->dec + v2.dec);
				break;
			default:
				return TypeError(v1->type, v2.type);
			}
			break;

		case Value::ARR:
			if (v2.type != Value::ARR)
				return TypeError(v1->type, v2.type);

			f.stack.back() = *v1; // cp
			f.stack.back().arr->insert(f.stack.back().arr->end(), v2.arr->begin(), v2.arr->end());
			break;
		}

		return Frame::Exit();
	}
}

namespace op_minus {
	const char* name = "-";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "- expected 2 values", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();
		Value* v1 = (Value*) f.stack.back().defer();


		auto TypeError = [&](Value::vtype t1, Value::vtype t2) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "invalid subtraction of "
								+ std::string(Value::typeName(t1)) + " & " + Value::typeName(t2), f.feed.lineNumber());
		};

		switch (v1->type) {
			case Value::OBJ: {

				Frame::Exit ev;
				const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
				f.stack.emplace_back(std::move(v2));
				if (!v1->obj->callMember(f, "__operator-", ev, self, 1))
					return Frame::Exit(Frame::Exit::ERROR, "in object.__operator-", DEBUG_FLI , f.feed.lineNumber(), ev);
				if (ev.reason == Frame::Exit::RETURN)
					return Frame::Exit();
				return ev;
			} break;
		case Value::DEC:
			switch (v2.type) {
				case Value::DEC:
					f.stack.back().set(v1->dec - v2.dec);
					break;
				case Value::INT:
					f.stack.back().set(v1->dec - v2.mp_int->get_d());
					break;
				default:
					return TypeError(v1->type, v2.type);
			}
			break;
		case Value::INT:
			switch (v2.type) {
			case Value::DEC:
				f.stack.back().set(v1->mp_int->get_d() - v2.dec);
				break;
			case Value::INT:
				f.stack.back().set(mpz_class(*v1->mp_int - *v2.mp_int));
				break;
			default:
				return TypeError(v1->type, v2.type);
			}
			break;
		}

		return Frame::Exit();
	}
}


// TODO: make the rest of these non-retarded (don't use DEFER_TOP())
//		 also add operator overloading


namespace op_multiply {
	const char* name = "*";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "* expected 2 values", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();
		Value* v1 = (Value*) f.stack.back().defer();


		auto TypeError = [&](Value::vtype t1, Value::vtype t2) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "invalid operands to * of types "
																+ std::string(Value::typeName(t1)) + " & " + Value::typeName(t2), f.feed.lineNumber());
		};

		switch (v1->type) {
			case Value::OBJ: {

				Frame::Exit ev;
				const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
				f.stack.emplace_back(std::move(v2));
				if (!v1->obj->callMember(f, "__operator*", ev, self, 1))
					return Frame::Exit(Frame::Exit::ERROR, "in .__operator*", DEBUG_FLI , f.feed.lineNumber(), ev);
				if (ev.reason == Frame::Exit::RETURN)
					return Frame::Exit();
				return ev;
			} break;
			case Value::DEC:
				switch (v2.type) {
					case Value::DEC:
						f.stack.back().set(v1->dec * v2.dec);
						break;
					case Value::INT:
						f.stack.back().set(v1->dec * v2.mp_int->get_d());
						break;
					default:
						return TypeError(v1->type, v2.type);
				}
				break;
			case Value::INT:
				switch (v2.type) {
					case Value::DEC:
						f.stack.back().set(v1->mp_int->get_d() * v2.dec);
						break;
					case Value::INT:
						f.stack.back().set(mpz_class(*v1->mp_int * *v2.mp_int));
						break;
					default:
						return TypeError(v1->type, v2.type);
				}
				break;
		}

		return Frame::Exit();
	}

}


static inline bool to_dec(const Value& v, double& d) {
	if (v.type == Value::INT) {
		d = v.mp_int->get_d();
		return true;
	} else if (v.type == Value::DEC) {
		d = v.dec;
		return true;
	}
	return false;
}
static inline bool to_int(const Value& v, mpz_class& i) {
	if (v.type == Value::INT) {
		i = *v.mp_int;
		return true;
	} else if (v.type == Value::DEC) {
		i = mpz_class(v.dec);
		return true;
	}
	return false;
}

namespace op_divide {
	const char* name = "/";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "/ expected 2 numbers", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();

		const Value* v = f.stack.back().defer();
		if (v && v->type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			f.stack.emplace_back(std::move(v2));
			if (!v->obj->callMember(f, "__operator/", ev, self, 1))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__operator/", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}

		if (!v)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Null/cyclic reference passed to operator /", f.feed.lineNumber());

		Value v1 = *v;
		auto TypeError = [&](Value::vtype t1, Value::vtype t2) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "invalid division of "
								+ std::string(Value::typeName(t1)) + " & " + Value::typeName(t2), f.feed.lineNumber());
		};

		double d1, d2;
		if (!to_dec(v1, d1) || !to_dec(v2, d2))
			return TypeError(v1.type, v2.type);

		f.stack.back().set(d1 / d2);

		return Frame::Exit();
	}
}

namespace op_int_divide {
	const char* name = "//";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "// expected 2 numbers", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();

		const Value* v = f.stack.back().defer();
		if (v && v->type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			f.stack.emplace_back(std::move(v2));
			if (!v->obj->callMember(f, "__operator//", ev, self, 1))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__operator//", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}

		if (!v)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Null/cyclic reference passed to operator //", f.feed.lineNumber());

		Value v1 = *v;
		auto TypeError = [&](Value::vtype t1, Value::vtype t2) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "invalid integer division of "
								+ std::string(Value::typeName(t1)) + " & " + Value::typeName(t2), f.feed.lineNumber());
		};
		mpz_class i1, i2;
		if (!to_int(v1, i1) || !to_int(v2, i2))
			return TypeError(v1.type, v2.type);

		f.stack.back().set(mpz_class(i1 / i2));

		return Frame::Exit();
	}
}

namespace op_pow {
	const char* name = "**";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "** expected 2 numbers", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();

		const Value* v = f.stack.back().defer();
		if (v && v->type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			f.stack.emplace_back(std::move(v2));
			if (!v->obj->callMember(f, "__operator**", ev, self, 1))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__operator**", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}

		if (!v)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Null/cyclic reference passed to operator **", f.feed.lineNumber());

		Value v1 = *v;
		if ((v1.type != Value::INT && v1.type != Value::DEC) && (v2.type != Value::INT && v2.type != Value::DEC))
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " ** expected 2 numbers, received "
				+ std::string(v1.typeName()) + " & "+ v2.typeName(), f.feed.lineNumber());

		// if both int then try mpz_pow_ui, else, conv to double
		if (v1.type == Value::INT && v2.type == Value::INT && *v2.mp_int > 0)
				mpz_pow_ui(f.stack.back().mp_int->get_mpz_t(), f.stack.back().mp_int->get_mpz_t(), v2.mp_int->get_ui());

		else
			f.stack.back().set(pow(
					f.stack.back().type == Value::INT ?
						f.stack.back().mp_int->get_d() : f.stack.back().dec,
					v2.type == Value::INT ?
						v2.mp_int->get_d() : v2.dec ));

		return Frame::Exit();

	}
}

namespace op_remainder {
	const char* name = "%";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {

		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "% expected 2 numbers", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();

		const Value* v = f.stack.back().defer();
		if (v && v->type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			f.stack.emplace_back(std::move(v2));
			if (!v->obj->callMember(f, "__operator%", ev, self, 1))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__operator%", DEBUG_FLI , f.feed.lineNumber(), ev);
			return ev;
		}

		if (!v)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Null/cyclic reference passed to operator %", f.feed.lineNumber());

		Value v1 = *v;

		if ((v1.type != Value::INT && v1.type != Value::DEC) && (v2.type != Value::INT && v2.type != Value::DEC))
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " % expected 2 numbers, received "
				+ std::string(v1.typeName()) + " & "+ v2.typeName(), f.feed.lineNumber());

		if (v1.type == Value::INT && v2.type == Value::INT)
			f.stack.back().set(mpz_class(*v1.mp_int % *v2.mp_int));
		else
			f.stack.back().set(std::fmod(
					(v1.type == Value::INT ?
						v1.mp_int->get_d() : v1.dec),
					(v2.type == Value::INT ?
						v2.mp_int->get_d() : v2.dec)));

		return Frame::Exit();
	}
}

namespace op_abs {
	const char* name = "abs";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " abs expected a number to take the absolute value of", f.feed.lineNumber());



		const Value* v = f.stack.back().defer();
		if (v && v->type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			if (!v->obj->callMember(f, "__abs", ev, self))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__abs", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}

		if (!v)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Null/cyclic reference passed to abs", f.feed.lineNumber());

		f.stack.back().set(*v);
		if (f.stack.back().type != Value::INT && f.stack.back().type != Value::DEC)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " abs expected a number to take the absolute value of, received " + std::string(f.stack.back().typeName()), f.feed.lineNumber());

		if (f.stack.back().type == Value::INT)
			abs(*f.stack.back().mp_int);
		else
			f.stack.back().set(std::abs(f.stack.back().dec));

		return Frame::Exit();
	}

}

namespace op_shift_left {
	const char* name = "<<";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " << expected 2 ints.", f.feed.lineNumber());

		auto TypeError = [&](Value::vtype t) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
							   DEBUG_FLI " << expected int, received " + std::string(Value::typeName(t)), f.feed.lineNumber());
		};


		Value v2 = f.stack.back();
		f.stack.pop_back();
		Value v1 = f.stack.back();

		const Value* p1 = v1.defer();

		if (p1->type == Value::OBJ) {
			Frame::Exit ev;
			f.stack.emplace_back(std::move(v2));
			if (!p1->obj->callMember(f, "__operator<<", ev, v1.lastRef(), 1))
				return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "in object.__operator<<", f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}


		const Value* p2 = v2.defer();
		if (!p2 || p2->type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_class n2 = *p2->mp_int;

		if (n2 < 0)
			return Frame::Exit(Frame::Exit::ERROR, "ValueError", DEBUG_FLI " negative shift");


		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_mul_2exp(f.stack.back().mp_int->get_mpz_t(), f.stack.back().mp_int->get_mpz_t(), n2.get_si());

		return Frame::Exit();
	}
}


namespace op_shift_right {
	const char* name = ">>";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " >> expected 2 ints.", f.feed.lineNumber());

		auto TypeError = [&](Value::vtype t) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
							   DEBUG_FLI " << expected int, received " + std::string(Value::typeName(t)), f.feed.lineNumber());
		};

		Value v2 = f.stack.back();
		f.stack.pop_back();
		Value v1 = f.stack.back();

		const Value* p1 = v1.defer();

		if (p1->type == Value::OBJ) {
			Frame::Exit ev;
			f.stack.emplace_back(std::move(v2));
			if (!p1->obj->callMember(f, "__operator<<", ev, v1.lastRef(), 1))
				return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "in object.__operator<<", f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}


		const Value* p2 = v2.defer();
		if (!p2 || p2->type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_class n2 = *p2->mp_int;

		if (n2 < 0)
			return Frame::Exit(Frame::Exit::ERROR, "ValueError", DEBUG_FLI " negative shift");


		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_div_2exp(f.stack.back().mp_int->get_mpz_t(), f.stack.back().mp_int->get_mpz_t(), n2.get_ui());

		return Frame::Exit();

	}
}



namespace op_bw_and {
	const char* name = "&";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "& expected 2 ints");

		auto TypeError = [&](Value::vtype t) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
							   DEBUG_FLI " << expected int, received " + std::string(Value::typeName(t)), f.feed.lineNumber());
		};

		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_class v = *f.stack.back().mp_int;
		f.stack.pop_back();

		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_and(f.stack.back().mp_int->get_mpz_t(), f.stack.back().mp_int->get_mpz_t(), v.get_mpz_t());

		return Frame::Exit();
	}
}


// or

namespace op_bw_xor {
	const char* name = "^";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "* expected 2 ints");

		auto TypeError = [&](Value::vtype t) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
							   DEBUG_FLI " << expected int, received " + std::string(Value::typeName(t)), f.feed.lineNumber());
		};

		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_class v = *f.stack.back().mp_int;
		f.stack.pop_back();

		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_xor(f.stack.back().mp_int->get_mpz_t(), f.stack.back().mp_int->get_mpz_t(), v.get_mpz_t());

		return Frame::Exit();
	}
}

namespace op_bw_or {
	const char* name = "|";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "| expected 2 ints");

		auto TypeError = [&](Value::vtype t) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
							   DEBUG_FLI " << expected int, received " + std::string(Value::typeName(t)), f.feed.lineNumber());
		};

		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_class v = *f.stack.back().mp_int;
		f.stack.pop_back();

		DEFER_TOP(f);

		if (f.stack.back().type != Value::INT)
			return TypeError(f.stack.back().type);

		mpz_ior(f.stack.back().mp_int->get_mpz_t(), f.stack.back().mp_int->get_mpz_t(), v.get_mpz_t());

		return Frame::Exit();
	}
}




// TODO: modified assignment operators ://////