# post-jail
A simple program that can be used as a login shell, which sends a POST query to a server containing a (short) challenge that is asked back to the user. Intended to be sending a text message challenge through APIs.

Configuration
========

Postjail is configured in `.postjailrc`, which should be in the working
directory in which postjail will be invoked (probably the home of the user).

The syntax is a very basic one: each line matches `\w*[^=]*\w*=\w*[^=]*\w*`,
where `\w = [ \t\n]`, that is, a tag and a value separated by an `=` sign,
possibly surrounded by whitespaces.

The tag may be one of the following:
* `url`: the address that must be called to send the challenge;
* `http_header`: any header you might want to specify in the HTTP request
 (e.g., `Content-Type`);
* `post_header` and `post_footer`: the text that surrounds the challenge string
 in the POST request;
* `on_success`: the program (not command!) to invoke on success;
* `challenge_length`: an integer specifying the challenge length;
* `cooldown_time`: the number of seconds that must ellapse between a
 failed authentication and the next authentication try.
 
A line is considered to be a comment (and thus ignored) if its first character
is a `#`.

This is an example of configuration file:
 
```
url = https://example.com/query_url
http_header = Content-Type: application/json
post_header = { "challenge":"
post_footer = " }
on_success = /bin/bash
challenge_length = 8
cooldown_time = 300
# 5 minutes cooldown time
```

