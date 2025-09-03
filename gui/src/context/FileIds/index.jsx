import React from "react";
import useFileIds from "../../hooks/useFileIds";

const FileIdsContext = React.createContext(null);

export const FileIdsProvider = ({ children }) => {
    const value = useFileIds();
    return (
        <FileIdsContext.Provider value={value}>
            {children}
        </FileIdsContext.Provider>
    );
};

export const useFileIdsContext = () => {
    return React.useContext(FileIdsContext);
};
