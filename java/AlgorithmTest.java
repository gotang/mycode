import edu.princeton.cs.algs4.*;

class AlgorithmTest {
	
	public static int mystery(int a, int b) {
		if (b == 0)
			return 1;
		if (b % 2 == 0)
			return mystery(a + a, b / 2);
		return mystery(a + a, b / 2) * a;
	}
	
	public static void mytest() {
		Integer a = 100;
		Integer b = 200;
		if(a == b) {
			StdOut.printf("%d\n",1);	
		}
	}
	
	public static void main(String[] arg) {
		System.out.println("AlgorithmTest");
		mytest();
	}
}
