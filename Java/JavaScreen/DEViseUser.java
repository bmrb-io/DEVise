class DEViseUser
{
    public long timeout = 30;
    public String username = DEViseGlobals.DEFAULTUSER;
    public String password = DEViseGlobals.DEFAULTPASS;
    public int priority = 0;
    
    public DEViseUser(String name, String pass, int p, long t)
    {   
        if (t >= 0)
            timeout = t;
       
        if (p >= 0) 
            priority = p;
        
        if (name != null)    
            username = name; 
            
        if (pass != null) 
            password = pass;
    }
    
    public boolean equals(DEViseUser user)
    {
        if (username.equals(user.username) && password.equals(user.password))
            return true;
        else
            return false;
    }        
}