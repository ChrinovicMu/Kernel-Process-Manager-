LWP-REQUEST(1p)            User Contributed Perl Documentation            LWP-REQUEST(1p)

NNAAMMEE
       lwp-request - Simple command line user agent

SSYYNNOOPPSSIISS
       llwwpp--rreeqquueesstt [--aaffPPuuUUssSSeeddvvhhxx] [--mm _m_e_t_h_o_d] [--bb _b_a_s_e _U_R_L] [--tt _t_i_m_e_o_u_t]
                   [--ii _i_f_-_m_o_d_i_f_i_e_d_-_s_i_n_c_e] [--cc _c_o_n_t_e_n_t_-_t_y_p_e]
                   [--CC _c_r_e_d_e_n_t_i_a_l_s] [--pp _p_r_o_x_y_-_u_r_l] [--oo _f_o_r_m_a_t] _u_r_l...

DDEESSCCRRIIPPTTIIOONN
       This program can be used to send requests to WWW servers and your local file
       system. The request content for POST and PUT methods is read from stdin.  The
       content of the response is printed on stdout.  Error messages are printed on
       stderr.  The program returns a status value indicating the number of URLs that
       failed.

       The options are:

       -m <method>
           Set which method to use for the request.  If this option is not used, then the
           method is derived from the name of the program.

       -f  Force request through, even if the program believes that the method is
           illegal.  The server might reject the request eventually.

       -b <uri>
           This URI will be used as the base URI for resolving all relative URIs given as
           argument.

       -t <timeout>
           Set the timeout value for the requests.  The timeout is the amount of time
           that the program will wait for a response from the remote server before it
           fails.  The default unit for the timeout value is seconds.  You might append
           "m" or "h" to the timeout value to make it minutes or hours, respectively.
           The default timeout is '3m', i.e. 3 minutes.

       -i <time>
           Set the If-Modified-Since header in the request. If _t_i_m_e is the name of a
           file, use the modification timestamp for this file. If _t_i_m_e is not a file, it
           is parsed as a literal date. Take a look at HTTP::Date for recognized formats.

       -c <content-type>
           Set the Content-Type for the request.  This option is only allowed for
           requests that take a content, i.e. POST and PUT.  You can force methods to
           take content by using the "-f" option together with "-c".  The default
           Content-Type for POST is "application/x-www-form-urlencoded".  The default
           Content-type for the others is "text/plain".

       -p <proxy-url>
           Set the proxy to be used for the requests.  The program also loads proxy
           settings from the environment.  You can disable this with the "-P" option.

       -P  Don't load proxy settings from environment.

       -H <header>
           Send this HTTP header with each request. You can specify several, e.g.:

               lwp-request \
                   -H 'Referer: http://other.url/' \
                   -H 'Host: somehost' \
                   http://this.url/

       -C <username>:<password>
           Provide credentials for documents that are protected by Basic Authentication.
           If the document is protected and you did not specify the username and password
           with this option, then you will be prompted to provide these values.

       The following options controls what is displayed by the program:

       -u  Print request method and absolute URL as requests are made.

       -U  Print request headers in addition to request method and absolute URL.

       -s  Print response status code.  This option is always on for HEAD requests.

       -S  Print response status chain. This shows redirect and authorization requests
           that are handled by the library.

       -e  Print response headers.  This option is always on for HEAD requests.

       -E  Print response status chain with full response headers.

       -d  Do nnoott print the content of the response.

       -o <format>
           Process HTML content in various ways before printing it.  If the content type
           of the response is not HTML, then this option has no effect.  The legal format
           values are; "text", "ps", "links", "html" and "dump".

           If you specify the "text" format then the HTML will be formatted as plain
           "latin1" text.  If you specify the "ps" format then it will be formatted as
           Postscript.

           The "links" format will output all links found in the HTML document.  Relative
           links will be expanded to absolute ones.

           The "html" format will reformat the HTML code and the "dump" format will just
           dump the HTML syntax tree.

           Note that the "HTML-Tree" distribution needs to be installed for this option
           to work.  In addition the "HTML-Format" distribution needs to be installed for
           "-o text" or "-o ps" to work.

       -v  Print the version number of the program and quit.

       -h  Print usage message and quit.

       -a  Set text(ascii) mode for content input and output.  If this option is not
           used, content input and output is done in binary mode.

       Because this program is implemented using the LWP library, it will only support
       the protocols that LWP supports.

SSEEEE AALLSSOO
       lwp-mirror, LWP

CCOOPPYYRRIIGGHHTT
       Copyright 1995-1999 Gisle Aas.

       This library is free software; you can redistribute it and/or modify it under the
       same terms as Perl itself.

AAUUTTHHOORR
       Gisle Aas <gisle@aas.no>

perl v5.36.0                            2023-03-01                        LWP-REQUEST(1p)
