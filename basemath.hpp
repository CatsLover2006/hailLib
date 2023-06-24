namespace hailMath {

	float abs_q(float x);

	double abs_q(double x);

	template <typename Number> Number abs (Number x) {
		if (x < 0) return -x;
		return x;
	}

    template <typename Number> Number sign(Number x) {
        if (!x) return 0;
        return x / abs<Number>(x);
    }

    extern double pi;

    template <typename Number> Number mod(Number x, Number m) {
        return x - floor(x/m)*m;
    }

    template <typename Number> Number sin_part_q(number x) {
        Number pi_t = (Number) pi;
        return (pi_t * pi_t - 4 * x * x) / (pi_t * pi_t + x * x);
    }

    template <typename Number> Number sin_q(number x) {
        x = mod<Number>(x, (Number) (2 * pi);
        return sin_part_q(mod<Number>(x, (Number)pi) - (Number)(pi/2)) * sign<Number>((Number)pi - x);
    }

	template <typename Number> Number constrain (Number x, Number l, Number t) {
		if (x < l) return l;
		if (x > t) return t;
		return x;
	}

	template <typename Number> Number max (Number a, Number b) {
		if (a > b) return a;
		return b;
	}

	template <typename Number> Number min (Number a, Number b) {
		if (a < b) return a;
		return b;
	}
	
	template <typename Number> Number lerp(Number a, Number b, Number t) {
		return a*t + (1-t)*b;
	}

}
