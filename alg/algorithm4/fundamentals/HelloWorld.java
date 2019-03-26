public class HelloWorld {
    public static void main(String[] args) { 
        System.out.printf("Hello, World %f\n", sqrt(3));
    }

    static double sqrt (double c) {
    	if (c < 0) return Double.NaN;
    	double err = 1e-15;

    	double t = c;
    	while (Math.abs(t - c/t) > err * t) {
    		System.out.printf("%f\n", t);
    		t = (c/t + t) / 2.0;
    	}

    	return t;
    }

    public static int abs(int x) {
		if (x < 0) return -x;
		else return x;
	}
}
