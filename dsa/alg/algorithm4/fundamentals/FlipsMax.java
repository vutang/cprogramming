/******************************************************************************
 *  Compilation:  javac FlipsMax.java
 *  Execution:    java FlipsMax n
 *  Dependencies: Counter.java StdRandom.java StdOut.java
 *
 *  A static method that takes two objects as arguments and returns
 *  an object.
 *
 *  % java FlipsMax 1000000
 *  500281 tails wins
 *
 ******************************************************************************/

public class FlipsMax {

    public static Counter max(Counter x, Counter y) {
        if (x.tally() > y.tally()) return x;
        else return y;
    }

    public static void main(String[] args) {
        int n = Integer.parseInt(args[0]);
        Counter heads = new Counter("heads");
        Counter tails = new Counter("tails");
        for (int i = 0; i < n; i++) {
            if (StdRandom.bernoulli(0.5)) heads.increment();
            else                          tails.increment();
        }

        if (heads.tally() == tails.tally())
            StdOut.println("Tie");
        else
            StdOut.println(max(heads, tails) + " wins");
    }
}

