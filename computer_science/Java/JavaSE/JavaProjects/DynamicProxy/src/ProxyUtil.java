import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

public class ProxyUtil {
    //给明星创建一个代理
    public static Star createProxy (BigStar bigStar) {
        //形参，被代理的明星对象
        //返回值，给明星创建的代理
        Star star = (Star)Proxy.newProxyInstance(
                ProxyUtil.class.getClassLoader(),
                new Class[]{Star.class},
                new InvocationHandler() {
                    @Override
                    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
                        if ("sing".equals(method.getName())) {
                            System.out.println("准备话筒，收钱");
                        }else if ("dance".equals(method.getName())) {
                            System.out.println("准备场地，收钱");
                        }

                        return method.invoke(bigStar, args);
                    }
                }
        );

        return star;
    }
}
