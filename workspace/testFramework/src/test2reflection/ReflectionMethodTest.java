package test2reflection;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

@SuppressWarnings("rawtypes")
public class ReflectionMethodTest {

	@SuppressWarnings("unused")
	public static void main(String[] args) {
	    String name = "java.util.Date";
	    try {
	      Class cl = Class.forName(name);
	      Class supercl = cl.getSuperclass();
	      System.out.println("class " + name);
	      System.out.println("Its methods:");
	      printMethods(cl);
	      System.out.println();
	    } catch (ClassNotFoundException e) {
	      System.out.println("Class not found.");
	    }
	  }
	
	
	public static void printMethods(Class cl) {
	    Method[] methods = cl.getDeclaredMethods();

	    for (int i = 0; i < methods.length; i++) {
	      Method m = methods[i];
	      Class retType = m.getReturnType();
	      Class[] paramTypes = m.getParameterTypes();
	      String name = m.getName();
	      System.out.print(Modifier.toString(m.getModifiers()));
	      System.out.print(" " + retType.getName() + " " + name + "(");
	      for (int j = 0; j < paramTypes.length; j++) {
	        if (j > 0)
	          System.out.print(", ");
	        System.out.print(paramTypes[j].getName());
	      }
	      System.out.println(");");
	    }
	  }
	}

