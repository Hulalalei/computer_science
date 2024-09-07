import smtplib
from email.mime.text import MIMEText
from email.utils import formataddr


if __name__ == "__main__":
    sender = '1537259139@qq.com'
    mail_host = "smtp.qq.com"
    mail_user = "1537259139@qq.com"
    mail_pass = "xgcrovbiuqtlhfcc"


    mail_msg = """
    <h2> testing </h2>
    """
    message  = MIMEText(mail_msg, 'html', 'utf-8')
    message['From'] = formataddr(["From_sender", sender])
    message['To'] = formataddr(["FK", mail_user])
    subject = 'title'
    message['Subject'] = subject
    try:
        smtpObj = smtplib.SMTP_SSL(mail_host, 465)
        smtpObj.login(mail_user, mail_pass)
        smtpObj.sendmail(sender, [mail_user, ], message.as_string())
        smtpObj.quit()
        print("send success")
    except smtplib.SMTPException:
        print("send error")
