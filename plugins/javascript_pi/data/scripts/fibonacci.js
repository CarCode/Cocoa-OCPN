function fibonacci(n) {

	function fib(n) {
		if (n == 0) return 0;
		if (n == 1) return 1;
		return fib(n-1) + fib(n-2);
		};
		
		var res = [];
		for (i = 0; i < n; i++) res.push(fib(i));
		return(res.join(' '));
		};