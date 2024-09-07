public class Users {
    private String userName;
    private String passWord;
    private String idCard;
    private String phoneNum;

    public Users() {
    }

    public Users(String userName, String passWord, String idCard, String phoneNum) {
        this.userName = userName;
        this.passWord = passWord;
        this.idCard = idCard;
        this.phoneNum = phoneNum;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getPassWord() {
        return passWord;
    }

    public void setPassWord(String passWord) {
        this.passWord = passWord;
    }

    public String getIdCard() {
        return idCard;
    }

    public void setIdCard(String idCard) {
        this.idCard = idCard;
    }

    public String getPhoneNum() {
        return phoneNum;
    }

    public void setPhoneNum(String phoneNum) {
        this.phoneNum = phoneNum;
    }
}
