#
# Regular cron jobs for the fexpr package.
#
0 4	* * *	root	[ -x /usr/bin/fexpr_maintenance ] && /usr/bin/fexpr_maintenance
