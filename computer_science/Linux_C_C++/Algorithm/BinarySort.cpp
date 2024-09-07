#include <iostream>
#include <algorithm>


const int N = 1e6 + 10;
int arr[N];
int n = 0, q = 0;


int main (void) {
	scanf ("%d %d", &n, &q);
	for (int i = 0; i < n; i++) scanf ("%d", &arr[i]);
	
	while (q--) {
		int x = 0;
		scanf ("%d", &x);

		int l = 0, r = n - 1;
		//区间[l, r]被分为[l, mid]和[mid + 1, r]
		//此时，答案前一区间内，因为mid满足
		while (l < r) {
			int mid = l + r >> 1;
			if (arr[mid] >= x) r = mid;
			else l = mid + 1;
		}

		if (arr[l] != x) std::cout << "-1, -1\n";
		else {
			std::cout << l << " ";
			
			int ls = 0, r = n - 1;
			//区间[l, r]被分为[l, mid - 1]和[mid, r]
			//此时，答案后一区间内，因为mid满足
			while (ls < r) {
				int mid = ls + r + 1 >> 1;
				if (arr[mid] <= x) ls = mid;
				else r = mid - 1;
			}
			std::cout << ls << "\n";
		}
	}
}

/*
#include <iostream>

using namespace std;

int main()
{
    double x;
    cin >> x;

    double l = -100, r = 100;
    while (r - l > 1e-8)
    {
        double mid = (l + r) / 2;
        if (mid * mid * mid >= x) r = mid;
        else l = mid;
    }

    printf("%.6lf\n", l);
    return 0;
}
*/

/*
#include <iostream>


int main(void) {
    double t = 0;
    std::cin >> t;
    
    double l = -100, r = 100;
    while (r - l > 1e-8) {
        double mid = (l + r) / 2;
        if (mid * mid * mid >= t) r = mid;
        else l = mid;
    }
    printf("%.6lf\n", l);
    
    return 0;
}
*/