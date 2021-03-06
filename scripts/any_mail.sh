#!/bin/bash
#.*
#   The second line is read by fake-smtp-server which used it as a regular expression.
#   It must start with a '#' and can not contain comments
#   The regular expression '.*' will be always valid.
#   You could also use, for example
#MAIL FROM: <sender@mail.com>
#   to execute this script when receiving mail from sender@mail.com
#RCPT TO: <receiver@mail.com>
#   to execute this script when receiving mail to receiver@mail.com
#Subject: Motion detected by camera
#   to execute this script when the subject of email is 'Motion detected by camera'
#   or any regular expression from your choice.
#
#   You could create as much script file as you want, but these must end in *.sh, and these must be in the execution directory of fake-smtp-server.
echo "Modify $0 to call command when receiving email."
touch lastEmailSeen

# Attachments are $1, $2, ...
# They could be overwritted if not copied
for attachment in "$@"
do
    mv "attachments/$attachment" "attachments/$(date +%F_%H-%M-%S)_$attachment"
done

#   This is an example to show you how you could send information to domoticz :
# curl -s http://domoticz:8080/json.htm?type=command&param=switchlight&idx=39&switchcmd=On&username=MkE=&password=OVM=
