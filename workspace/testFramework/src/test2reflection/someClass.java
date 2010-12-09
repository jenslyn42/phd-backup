package test2reflection;

public class someClass{
	public int one = 1;

	public someClass()
	{
		
	}
	public int methodOne(String a, int b){System.out.println(a+" "+one+":"+b); return 5;}

	public int otherMethodOne(String a){System.out.println(a+" "+one); return 5;}

	public String methodThree(int a, String[] b)
	{
		System.out.print(a+" ");
		for(String item : b){
			System.out.print(item);
		}
		System.out.println(" ");
		
		return "worked "+a;
	}
}
